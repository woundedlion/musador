#include <string>
#include "LibrarianController.h"
#include "Library.h"
#include "StatsBlock.h"
#include "Server/HTTP.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "Config/Config.h"

using namespace Musador;

LibrarianController::Error&
LibrarianController::Error::operator<<(const std::wstring& s)
{
    this->errStr << Util::unicodeToUtf8(s);
    return *this;
}

LibrarianController::Error&
LibrarianController::Error::operator<<(const std::string& s)
{
    this->errStr << s;
    return *this;
}

#define BIND_HANDLER(uri,handler) this->addHandler(uri,boost::bind(&LibrarianController::handler,this,_1))

LibrarianController::LibrarianController()
{
	BIND_HANDLER("/info",info);
	BIND_HANDLER("/config",config);
	BIND_HANDLER("/index",index);
    BIND_HANDLER("/library.xml",getLibraryXML);
    BIND_HANDLER("/library_stats.xml",getLibraryStatsXML);
}

bool 
LibrarianController::info(HTTP::Env& env)
{
	env.res->data.reset(new std::stringstream);
	env.req->dump(*env.res->data);
	env.res->headers["Content-Type"] = "text/html";
	env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());
	return true;
}

bool 
LibrarianController::config(HTTP::Env& env)
{
	env.res->data.reset(new std::stringstream);
	{
		boost::archive::xml_oarchive ar(*env.res->data);
		ar << boost::serialization::make_nvp("Librarian",*Config::instance());
	}
	env.res->headers["Content-Type"] = "text/xml";
	env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());
	return true;
}

bool
LibrarianController::index(HTTP::Env& env)
{
	std::string& path = env.req->params["path"];
	return false;
}

bool
LibrarianController::getLibraryStatsXML(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);

    std::string& libIDStr = env.req->params["lib_id"];
    LibrarianConfig::LibraryCollection libraries = Config::instance()->librarian.libraries;
    int libID;
    if (libIDStr.empty())
    {
        // Serialize all the libraries
        std::vector<StatsBlock> stats;
        for (LibrarianConfig::LibraryCollection::iterator iter = libraries.begin(); iter != libraries.end(); ++iter)
        {
            Library lib(iter->second);
            stats.push_back(lib.getCountsByGenre());
        }
        boost::archive::xml_oarchive ar(*env.res->data);
        ar << boost::serialization::make_nvp("libraries", stats);
    }
    else
    {
        // Serialize a single library
        try 
        {
            libID = boost::lexical_cast<int>(libIDStr);
            LibrarianConfig::LibraryCollection::iterator iter = libraries.find(libID);
            boost::archive::xml_oarchive ar(*env.res->data);
            if (iter != libraries.end())
            {
                // Good library ID, serialize
                Library lib(iter->second);
                ar << boost::serialization::make_nvp("library", lib.getCountsByGenre());
            }
            else
            {
                // Library with that ID does not exists
                Error err;
                err << "Invalid library ID";
                ar << boost::serialization::make_nvp("error",err);
            }
        }
        catch (const boost::bad_lexical_cast& e)
        {
            // Bad library ID
            boost::archive::xml_oarchive ar(*env.res->data);
            Error err;
            err << "Invalid library ID [" << e.what() << "]";
            ar << boost::serialization::make_nvp("error",err);        
        }
    }

    env.res->headers["Content-Type"] = "text/xml";
    env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());

    return true;
}

bool
LibrarianController::getLibraryXML(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);

    std::string& libIDStr = env.req->params["lib_id"];
    LibrarianConfig::LibraryCollection libraries = Config::instance()->librarian.libraries;
    int libID;
    if (libIDStr.empty())
    {
        // Serialize all the libraries
        boost::archive::xml_oarchive ar(*env.res->data);
        ar << boost::serialization::make_nvp("libraries",libraries);
    }
    else
    {
        // Serialize a single library
        try 
        {
            libID = boost::lexical_cast<int>(libIDStr);
            LibrarianConfig::LibraryCollection::iterator iter = libraries.find(libID);
            boost::archive::xml_oarchive ar(*env.res->data);
            if (iter != libraries.end())
            {
                // Good library ID, serialize
                ar << boost::serialization::make_nvp("library",*iter);
            }
            else
            {
                // Library with that ID does not exists
                Error err;
                err << "Invalid library ID";
                ar << boost::serialization::make_nvp("error",err);
            }
        }
        catch (const boost::bad_lexical_cast& e)
        {
            // Bad library ID
            boost::archive::xml_oarchive ar(*env.res->data);
            Error err;
            err << "Invalid library ID [" << e.what() << "]";
            ar << boost::serialization::make_nvp("error",err);        
        }
    }

    env.res->headers["Content-Type"] = "text/xml";
    env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());

    return true;
}