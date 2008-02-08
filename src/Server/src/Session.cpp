#include "Session.h"

using namespace Musador;

//////////////////////////////////////////////////////////////////////////////////////
// Session
//////////////////////////////////////////////////////////////////////////////////////
Session::Session() 
{
}

Session::~Session() 
{
}

std::string& 
Session::operator[](const std::string& key) 
{	
	Guard guard(this->lock);
	std::string& rv = this->store[key];
	return rv;
}

void 
Session::clear() 
{
	Guard(this->lock);
	this->store.clear();
}

std::string
Session::getName()
{
	return this->name;
}

void
Session::setName(const std::string& name)
{
	this->name = name;
}