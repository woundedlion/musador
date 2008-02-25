#include "LibrarianController.h"
#include "Server/HTTP.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"

using namespace Musador;

#define BIND_HANDLER(uri,handler) this->addHandler(uri,boost::bind(&LibrarianController::handler,this,_1))

LibrarianController::LibrarianController()
{
	BIND_HANDLER("/info",info);
}

bool 
LibrarianController::info(HTTP::Env & env)
{
	HTTP::Request& req = *env.req;
	env.req->dump(env.res->data);
	env.res->headers["Content-Type"] = "text/html";
	env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(env.res->data.tellp());
	return true;
}