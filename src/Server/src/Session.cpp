#include "Session.h"

using namespace Musador;

//////////////////////////////////////////////////////////////////////////////////////
// Session
//////////////////////////////////////////////////////////////////////////////////////
Session::Session(const std::string& key) :
key(key)
{
}

Session::~Session() 
{
}

void 
Session::clear() 
{
    Guard(this->lock);
    this->store.clear();
}

std::string
Session::getKey() const
{
    return this->key;
}