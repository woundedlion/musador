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
#include "ConnectionProcessor.h"

namespace Musador
{

	using namespace Musador;

	class Server;

	class ConnectionCtx
	{
	public:
		
		ConnectionCtx() :
		server(NULL)
		{}

		Server * server;
	};

	class Connection  : public boost::enable_shared_from_this<Connection>
	{
	public:

		Connection();

		Connection(SOCKET socket);

		virtual ~Connection();

		SOCKET getSocket();
		
		void setSocket(SOCKET s);

		sockaddr_in getLocalEP();
		
		void setLocalEP(sockaddr_in localEP);

		sockaddr_in getRemoteEP();

		void setRemoteEP(sockaddr_in localEP);

		boost::shared_ptr<ConnectionCtx> getCtx();
		virtual void setCtx(boost::shared_ptr<ConnectionCtx>);

		void close();

		std::string toString();

		void beginRead();
		void beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead);

		void beginWrite(boost::shared_array<char> data, unsigned int len);
		void beginWrite(const std::string& str);
		void beginWrite(std::stringstream& dataStream);

		virtual void accepted() = 0;
		virtual void post(boost::shared_ptr<IOMsgReadComplete> msgRead) = 0;
		virtual void post(boost::shared_ptr<IOMsgWriteComplete> msgWrite) = 0;

	protected:

		boost::shared_ptr<ConnectionCtx> ctx;
		sockaddr_in localEP;
		sockaddr_in remoteEP;
		SOCKET sock;
	};

	typedef AbstractFactory<Connection> ConnectionFactory;


}

#endif
