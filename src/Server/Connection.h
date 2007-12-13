#ifndef CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Network/Network.h"
#include "Protocol.h"
#include "IOMessages.h"


namespace Musador
{

	class Connection  : public boost::enable_shared_from_this<Connection>
	{
	public:

		Connection();

		Connection(SOCKET socket);

		~Connection();

		SOCKET getSocket();
		
		void setSocket(SOCKET s);

		sockaddr_in getLocalEP();
		
		void setLocalEP(sockaddr_in localEP);

		sockaddr_in getRemoteEP();

		void setRemoteEP(sockaddr_in localEP);

		boost::shared_ptr<Protocol> getProtocol();

		void setProtocol(boost::shared_ptr<Protocol> protocol);

		std::string toString();

	private:

		sockaddr_in localEP;
		sockaddr_in remoteEP;
		SOCKET sock;
		boost::shared_ptr<Protocol> protocol;

	};

}

#endif