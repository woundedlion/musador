#include "SocketListener.h"
#include "SocketConnection.h"
#include "Proactor.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"
using namespace Musador;

SocketListener::SocketListener(const sockaddr_in& localEP, 
							   int socketType /* = SOCK_STREAM */, 
							   int socketProto /* = IPPROTO_TCP */) :
localEP(localEP)
{
	Network& net = *Network::instance();

	// Set up server socket
	try
	{
		this->sock = net.socket(localEP.sin_family,socketType,socketProto);
		net.bind(this->sock,const_cast<sockaddr_in *>(&localEP));
		net.listen(this->sock);
	}
	catch (const NetworkException& e)
	{
		LOG(Error) << e.what();
		return;
	}

}

SocketListener::~SocketListener()
{
	this->close();
}

void
SocketListener::beginAccept(EventHandler handler, boost::any tag /* = NULL */)
{
	Proactor::instance()->beginAccept(this->shared_from_this(), handler, tag);
	LOG(Debug) << "Accepting connections on " << ::inet_ntoa(this->localEP.sin_addr) << ":" << ::ntohs(this->localEP.sin_port) 
			   << " [" << this->sock << "]";
}

void 
SocketListener::close()
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

