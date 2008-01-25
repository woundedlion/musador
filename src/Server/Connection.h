#ifndef CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <sstream>
#include "boost/function.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "boost/any.hpp"
#include "Network/Network.h"
#include "Utilities/Factory.h"
#include "IOMessages.h"

namespace Musador
{

	class Connection  : public boost::enable_shared_from_this<Connection>
	{
	public:

		typedef boost::function<void (boost::shared_ptr<IOMsgError>) > ErrorHandler;

		Connection();

		Connection(SOCKET socket);

		~Connection();

		SOCKET getSocket();
		
		void setSocket(SOCKET s);

		sockaddr_in getLocalEP();
		
		void setLocalEP(sockaddr_in localEP);

		sockaddr_in getRemoteEP();

		void setRemoteEP(sockaddr_in localEP);

		void setErrorHandler(ErrorHandler errorHandler);

		std::string toString();

		void beginRead();
		void beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead);

		void beginWrite(boost::shared_array<char> data, unsigned int len);
		void beginWrite(const std::string& str);
		void beginWrite(std::stringstream& dataStream);

		void onRead(boost::shared_ptr<IOMsg> msg, boost::any tag);

		void onWrite(boost::shared_ptr<IOMsg> msg, boost::any tag);

		virtual void accepted() = 0;
		virtual void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead) = 0;

	private:

		ErrorHandler errorHandler;
		sockaddr_in localEP;
		sockaddr_in remoteEP;
		SOCKET sock;

	};

	typedef AbstractFactory<Connection> ConnectionFactory;


}

#endif