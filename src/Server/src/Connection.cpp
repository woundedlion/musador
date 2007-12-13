#define NOMINMAX

#include <algorithm>
#include "boost/lexical_cast.hpp"
#include "Network/Network.h"
#include "boost/shared_ptr.hpp"
#include "Connection.h"
#include "Logger/Logger.h"
#define LOG(LVL) Musador::LoggerConsole::instance()->log(Musador::LVL,"Proactor")

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
			LOG(Info) << " Closing socket " << this->sock;
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

boost::shared_ptr<Protocol> Connection::getProtocol()
{
	return this->protocol;
}

void Connection::setProtocol(boost::shared_ptr<Protocol> protocol)
{
	this->protocol = protocol;
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