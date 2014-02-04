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

LibrarianController::LibrarianController()
{
	loadConfig();
	bindHandlers();
}

void
LibrarianController::loadConfig()
{
	LibrarianConfig::LibraryCollection libraries = Config::instance()->librarian.libraries;
	LibrarianConfig::LibraryCollection::iterator iter = libraries.find(LibrarianConfig::LOCAL_LIB_ID);
	if (iter != libraries.end()) {
		indexer.reset(new Indexer(iter->second.dataFile));
		std::vector<std::wstring> targets = iter->second.targets;
		for (auto target : targets) {
			indexer->addTarget(target);
		}
	}
	else
	{
		LOG(Error) << "Local library missing!";
	}
}

#define BIND_HANDLER(method, uri, handler) addHandler(method, uri, boost::bind(&LibrarianController::handler,this,_1))

void
LibrarianController::bindHandlers()
{
	BIND_HANDLER("GET", "/debug", dumpRequest);
	BIND_HANDLER("GET", "/config", getCongig);

#if 0	
    BIND_HANDLER("GET", "/index/start", reindex);
	BIND_HANDLER("GET", "/index/cancel", cancelIndex);
	BIND_HANDLER("GET", "/config/xml", getConfigXML);
	BIND_HANDLER("GET", "/index/progress/xml", getIndexProgressXML);
	BIND_HANDLER("GET", "/library/list/xml", getLibraryXML);
	BIND_HANDLER("GET", "/library/stats/xml", getLibraryStatsXML);
#endif
}

bool 
LibrarianController::dumpRequest(HTTP::Env& env)
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

    if (NULL != indexer) {
        // Don't reindex if we already are reindexing
        if (indexer->isRunning())
        {
            boost::archive::xml_oarchive ar(*env.res->data);
            Error error;
            error << "Index in progress";
            ar << BOOST_SERIALIZATION_NVP(error);
        } else {
            // Initiate a reindex
            indexer->reindex();
            boost::archive::xml_oarchive ar(*env.res->data);
            Success success;
            success << "Initiated library reindex";
            ar << BOOST_SERIALIZATION_NVP(success);
        }
    } else {
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

    if (NULL != indexer) {
        indexer->cancel();
        boost::archive::xml_oarchive ar(*env.res->data);
        Success success;
        success << "Canceled library reindex";
        ar << BOOST_SERIALIZATION_NVP(success);
    } else {
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
    env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());
    return true;
}

bool
LibrarianController::getIndexProgressXML(HTTP::Env& env)
{
    env.res->data.reset(new std::stringstream);
    if (NULL != indexer) {
        boost::archive::xml_oarchive ar(*env.res->data);
        ar << boost::serialization::make_nvp("IndexProgress", indexer->getProgress());
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
    if (libIDStr.empty()) {
        // Serialize all the libraries
        boost::archive::xml_oarchive ar(*env.res->data);
        ar << boost::serialization::make_nvp("libraries",libraries);
    } else {
        // Serialize a single library
        try {
            libID = boost::lexical_cast<int>(libIDStr);
            LibrarianConfig::LibraryCollection::iterator iter = libraries.find(libID);
            boost::archive::xml_oarchive ar(*env.res->data);
            if (iter != libraries.end()) {
                // Good library ID, serialize
                ar << boost::serialization::make_nvp("library",*iter);
            } else {
                // Library with that ID does not exist
                Error err;
                err << "Invalid library ID";
                ar << boost::serialization::make_nvp("error",err);
            }
        } catch (const boost::bad_lexical_cast& e) {
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
    if (libIDStr.empty()) {
        // Serialize all the libraries
        std::vector<StatsBlock> stats;
        for (auto libRec : libraries)
        {
            Library lib(libRec.second);
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
