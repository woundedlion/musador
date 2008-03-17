#define NOMINMAX

#include <algorithm>
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "Network/Network.h"
#include "boost/shared_ptr.hpp"
#include "Connection.h"
#include "ConnectionProcessor.h"
#include "Proactor.h"
#include "Server.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"
using namespace Musador;

//////////////////////////////////////////////////////////////////////////////////////
// Connection
//////////////////////////////////////////////////////////////////////////////////////

Connection::Connection() 
{

}

Connection::~Connection()
{
}

boost::shared_ptr<ConnectionCtx>
Connection::getCtx()
{
	return this->ctx;
}

void 
Connection::setCtx(boost::shared_ptr<ConnectionCtx> ctx)
{
	this->ctx = ctx;
}


