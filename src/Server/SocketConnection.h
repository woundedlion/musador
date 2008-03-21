#ifndef SOCKET_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define SOCKET_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"

namespace Musador
{

	class SocketConnection : public Connection, public boost::enable_shared_from_this<SocketConnection>
	{
	public:

		SocketConnection();

		~SocketConnection();

		void close();

		void beginRead(boost::any tag = NULL);
		void beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead,
                               boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
				boost::any tag = NULL);
		void beginWrite(boost::shared_array<char> data, unsigned int len, boost::any tag = NULL);
		void beginWrite(std::istream& dataStream, boost::any tag = NULL);
		void beginWrite(const std::string& str, boost::any tag = NULL);

		std::string toString();

		SOCKET getSocket();

		void setSocket(SOCKET sock);

		sockaddr_in getLocalEP();

		void setLocalEP(sockaddr_in localEP);

		sockaddr_in getRemoteEP();

		void setRemoteEP(sockaddr_in localEP);

	protected:

		sockaddr_in localEP;
		sockaddr_in remoteEP;
		SOCKET sock;

	};
}

#endif