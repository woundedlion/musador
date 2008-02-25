#define NOMINMAX

#include <algorithm>
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "Network/Network.h"
#include "boost/shared_ptr.hpp"
#include "Connection.h"
#include "ConnectionProcessor.h"
#include "Logger/Logger.h"
#include "Proactor.h"
#include "Server.h"
#define LOG_SENDER L"Proactor"

using namespace Musador;

//////////////////////////////////////////////////////////////////////////////////////
// Connection
//////////////////////////////////////////////////////////////////////////////////////

Connection::Connection() :
sock(NULL)
{

}

Connection::Connection(SOCKET clientSocket) :
sock(clientSocket)
{

}

Connection::~Connection()
{
	this->close();
}

SOCKET 
Connection::getSocket()
{
	return this->sock;
}

void 
Connection::setSocket(SOCKET s)
{
	this->sock = s;
}

sockaddr_in 
Connection::getLocalEP()
{
	return this->localEP;
}

void 
Connection::setLocalEP(sockaddr_in localEP)
{
	this->localEP = localEP;
}

sockaddr_in 
Connection::getRemoteEP()
{
	return this->remoteEP;
}

void 
Connection::setRemoteEP(sockaddr_in remoteEP)
{
	this->remoteEP = remoteEP;
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

void Connection::close()
{
	if (NULL != this->sock)
	{
		try
		{
			LOG(Debug) << " Closing socket " << this->sock;
			Network::instance()->closeSocket(this->sock);
			this->sock = NULL;
		}
		catch(const NetworkException& e)
		{
			LOG(Warning) << e.what() << " while closing client socket.";
		}
	}
}

std::string Connection::toString()
{
	std::stringstream r;
	r << ::inet_ntoa(this->remoteEP.sin_addr) << ":" << ::ntohs(this->remoteEP.sin_port) << " => " 
	  << ::inet_ntoa(this->localEP.sin_addr) << ":" << ::ntohs(this->localEP.sin_port)
	  << " [" << this->getSocket() << "]";
	return r.str();
}

void Connection::beginRead()
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2));
}

void Connection::beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), msgRead);
}

void Connection::beginWrite(boost::shared_array<char> data, unsigned int len)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), data, len);
}

void Connection::beginWrite(const std::string& str)
{
	unsigned int len = str.size();
	boost::shared_array<char> data(new char[len]);
	str.copy(data.get(), len);
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), data, len);
}

void Connection::beginWrite(std::stringstream& dataStream)
{
	unsigned int len = dataStream.tellp();
	if (-1 != static_cast<std::stringstream::pos_type>(len))
	{
		boost::shared_array<char> data(new char[len]);
		dataStream.str().copy(data.get(),len);
		Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&ConnectionProcessor::post,this->ctx->processor,_1,_2), data, len);
	}
}
