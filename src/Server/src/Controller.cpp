#include "Controller.h"
#include "HTTP.h"

using namespace Musador;

void 
Controller::addHandler(const std::string& requestURI, Handler handler)
{
    this->handlers[requestURI] = handler;
}

bool
Controller::exec(HTTP::Env& env)
{
    if (this->handlers.find(env.req->requestURI) != this->handlers.end())
    {
        return this->handlers[env.req->requestURI](env);
    }
    return false;
}