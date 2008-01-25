#define NOMINMAX

#include <algorithm>
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include "Network/Network.h"
#include "boost/shared_ptr.hpp"
#include "Connection.h"
#include "Logger/Logger.h"
#include "Proactor.h"
#define LOG_SENDER L"Proactor"

using namespace Musador;

//////////////////////////////////////////////////////////////////////////////////////
// Connection
//////////////////////////////////////////////////////////////////////////////////////

Connection::Connection()
{

}

Connection::Connection(SOCKET clientSocket) :
sock(clientSocket)
{

}

Connection::~Connection()
{
	if (NULL != this->sock)
	{
		try
		{
			LOG(Debug) << " Closing socket " << this->sock;
			Network::instance()->closeSocket(this->sock);
		}
		catch(const NetworkException& e)
		{
			LOG(Warning) << e.what() << " while closing client socket.";
		}
	}
}

SOCKET Connection::getSocket()
{
	return this->sock;
}

void Connection::setSocket(SOCKET s)
{
	this->sock = s;
}

sockaddr_in Connection::getLocalEP()
{
	return this->localEP;
}

void Connection::setLocalEP(sockaddr_in localEP)
{
	this->localEP = localEP;
}

sockaddr_in Connection::getRemoteEP()
{
	return this->remoteEP;
}

void Connection::setRemoteEP(sockaddr_in remoteEP)
{
	this->remoteEP = remoteEP;
}

void Connection::setErrorHandler(ErrorHandler errorHandler)
{
	this->errorHandler = errorHandler;
}

std::string Connection::toString()
{
	std::string r;
	r += ::inet_ntoa(this->remoteEP.sin_addr);
	r += ":";
	r += boost::lexical_cast<std::string>(::ntohs(this->remoteEP.sin_port));
	r += " => ";
	r += ::inet_ntoa(this->localEP.sin_addr);
	r += ":";
	r += boost::lexical_cast<std::string>(::ntohs(this->localEP.sin_port));
	return r;
}

void Connection::beginRead()
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onRead,this,_1,_2));
}

void Connection::beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onRead,this,_1,_2), msgRead);
}

void Connection::beginWrite(boost::shared_array<char> data, unsigned int len)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onRead,this,_1,_2), data, len);
}

void Connection::beginWrite(const std::string& str)
{
	unsigned int len = str.size();
	boost::shared_array<char> data(new char[len]);
	str.copy(data.get(), len);
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWrite,this,_1,_2), data, len);
}

void Connection::beginWrite(std::stringstream& dataStream)
{
	unsigned int len = dataStream.tellp();
	if (-1 != static_cast<std::stringstream::pos_type>(len))
	{
		boost::shared_array<char> data(new char[len]);
		dataStream.str().copy(data.get(),len);
		Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWrite,this,_1,_2), data, len);
	}
}

void Connection::onRead(boost::shared_ptr<IOMsg> msg, boost::any tag)
{
	switch (msg->getType())
	{
	case IO_READ_COMPLETE:
		{
			boost::shared_ptr<IOMsgReadComplete> msgRead(boost::shared_static_cast<IOMsgReadComplete>(msg));
			*this << msgRead;
		}
		break;
	case IO_ERROR:
		{
			if (NULL != this->errorHandler)
			{
				boost::shared_ptr<IOMsgError> msgErr(boost::shared_static_cast<IOMsgError>(msg));
				this->errorHandler(msgErr);
			}
		}
		break;
	}
}

void Connection::onWrite(boost::shared_ptr<IOMsg> msg, boost::any tag)
{
	switch (msg->getType())
	{
	case IO_WRITE_COMPLETE:
		{
		}
		break;
	case IO_ERROR:
		{
			if (NULL != this->errorHandler)
			{
				boost::shared_ptr<IOMsgError> msgErr(boost::shared_static_cast<IOMsgError>(msg));
				this->errorHandler(msgErr);
			}
		}
		break;
	}
}