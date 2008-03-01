#include "LibrarianController.h"
#include "Server/HTTP.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "Config/Config.h"

using namespace Musador;

#define BIND_HANDLER(uri,handler) this->addHandler(uri,boost::bind(&LibrarianController::handler,this,_1))

LibrarianController::LibrarianController()
{
	BIND_HANDLER("/info",info);
	BIND_HANDLER("/config",config);
	BIND_HANDLER("/index",index);
}

bool 
LibrarianController::info(HTTP::Env & env)
{
	env.res->data.reset(new std::stringstream);
	env.req->dump(*env.res->data);
	env.res->headers["Content-Type"] = "text/html";
	env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data->tellp());
	return true;
}

bool 
LibrarianController::config(HTTP::Env & env)
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
LibrarianController::index(HTTP::Env & env)
{
	std::string& path = env.req->params["path"];



	return false;
}