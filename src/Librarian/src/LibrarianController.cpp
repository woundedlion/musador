#include <string>
#include "LibrarianController.h"
#include "Library.h"
#include "StatsBlock.h"
#include "Server/HTTP.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "Config/Config.h"
#include "Logger/Logger.h"
#define LOG_SENDER "Controller"

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

LibrarianController::Success&
LibrarianController::Success::operator<<(const std::wstring& s)
{
    this->succStr << Util::unicodeToUtf8(s);
    return *this;
}

LibrarianController::Success&
LibrarianController::Success::operator<<(const std::string& s)
{
    this->succStr << s;
    return *this;
}

#define BIND_HANDLER(uri,handler) this->addHandler(uri,boost::bind(&LibrarianController::handler,this,_1))

LibrarianController::LibrarianController()
{
	BIND_HANDLER("/info",info);
	BIND_HANDLER("/reindex",reindex);
    BIND_HANDLER("/cancel_index",cancelIndex);
    BIND_HANDLER("/config.xml",getConfigXML);
    BIND_HANDLER("/index_progress.xml",getIndexProgressXML);
    BIND_HANDLER("/library.xml",getLibraryXML);
    BIND_HANDLER("/library_stats.xml",getLibraryStatsXML);

    LibrarianConfig::LibraryCollection libraries = Config::instance()->librarian.libraries;
    LibrarianConfig::LibraryCollection::iterator iter = libraries.find(LibrarianConfig::LOCAL_LIB_ID);
    if (iter != libraries.end())
    {   
        this->indexer.reset(new Indexer(iter->second.dataFile));
        std::vector<std::wstring> targets = iter->second.targets;
        for (std::vector<std::wstring>::const_iterator target = targets.begin(); 
            target != targets.end(); 
            ++target)
        {
            this->indexer->addRootTarget(*target);
        }
    }
    else
    {
        LOG(Error) << "Local library missing!";
    }
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
LibrarianController::reindex(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);

    if (NULL != this->indexer)
    {
        // Don't reindex if we already are reindexing
        if (this->indexer->isRunning())
        {
            boost::archive::xml_oarchive ar(*env.res->data);
            Error error;
            error << "Index in progress";
            ar << BOOST_SERIALIZATION_NVP(error);
        }
        else
        {
            // Initiate a reindex
            this->indexer->reindex();
            boost::archive::xml_oarchive ar(*env.res->data);
            Success success;
            success << "Initiated library reindex";
            ar << BOOST_SERIALIZATION_NVP(success);
        }
    }
    else
    {
        boost::archive::xml_oarchive ar(*env.res->data);
        Error error;
        error << "Local library missing!";
        ar << BOOST_SERIALIZATION_NVP(error);
    }

    env.res->headers["Content-Type"] = "text/xml";
    env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());

    return true;
}

bool
LibrarianController::cancelIndex(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);

    if (NULL != this->indexer)
    {
        this->indexer->cancel();
        boost::archive::xml_oarchive ar(*env.res->data);
        Success success;
        success << "Canceled library reindex";
        ar << BOOST_SERIALIZATION_NVP(success);
    }
    else
    {
        boost::archive::xml_oarchive ar(*env.res->data);
        Error error;
        error << "Local library missing!";
        ar << BOOST_SERIALIZATION_NVP(error);
    }

    env.res->headers["Content-Type"] = "text/xml";
    env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());

    return true;    
}

bool 
LibrarianController::getConfigXML(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);
    {
        // TODO: implement HTTP::Response::getXMLArchive
        boost::archive::xml_oarchive ar(*env.res->data);
        ar << boost::serialization::make_nvp("Librarian",*Config::instance());
    }
    env.res->headers["Content-Type"] = "text/xml";
    // TODO: implement default content-length
    env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());
    return true;
}

bool
LibrarianController::getIndexProgressXML(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);
    if (NULL != this->indexer)
    {
        boost::archive::xml_oarchive ar(*env.res->data);
        ar << boost::serialization::make_nvp("IndexProgress",this->indexer->progress());
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
