#include "Controller.h"
#include "HTTP.h"

using namespace Musador;

void 
Controller::addHandler(const std::string& requestURI, Handler handler)
{
    handlers[requestURI] = handler;
}

bool
Controller::exec(HTTP::Env& env)
{
    if (handlers.find(env.req->requestURI) != handlers.end()) {
        return handlers[env.req->requestURI](env);
    }
    return false;
}