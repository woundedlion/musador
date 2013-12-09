#include "Controller.h"

using namespace Musador;

void 
Controller::addHandler(const std::string& method, const std::string& requestURI, Handler handler)
{
    handlers[method][requestURI] = handler;
}

bool
Controller::exec(HTTP::Env& env)
{
	MethodMap::const_iterator method = handlers.find(env.req->method);
	if (method != handlers.end()) {
		HandlerMap::const_iterator handler = method->second.find(env.req->requestURI);
		if (handler != method->second.end()) {
			return (handler->second)(env);
		}
	}
    return false;
}