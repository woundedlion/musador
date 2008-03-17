#ifndef CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <sstream>
#include "boost/function.hpp"
#include <boost/shared_ptr.hpp>
#include "boost/any.hpp"
#include "Network/Network.h"
#include "Utilities/Factory.h"
#include "IOMessages.h"
#include "ConnectionProcessor.h"

namespace Musador
{

	using namespace Musador;

	class Server;
	class ConnectionProcessor;

	typedef AbstractFactory<Connection> ConnectionFactory;

	class ConnectionCtx
	{
	public:
		
		ConnectionCtx() :
			server(NULL),
			processor(NULL)
		{}

		Server * server;
		ConnectionProcessor * processor;
	};

	class Connection
	{
	public:

		Connection();

		virtual ~Connection();

		boost::shared_ptr<ConnectionCtx> getCtx();
		virtual void setCtx(boost::shared_ptr<ConnectionCtx>);

		virtual void beginRead() = 0;
		virtual void beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead) = 0;

		virtual void beginWrite(EventHandler handler, 
								boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
								boost::any tag /* = NULL */) = 0;
		virtual void beginWrite(boost::shared_array<char> data, unsigned int len) = 0;
		virtual void beginWrite(std::istream& dataStream) = 0;
		virtual void beginWrite(const std::string& str) = 0;

		virtual void accepted() = 0;

		virtual void close() = 0;

		virtual std::string toString() = 0;

		virtual void post(boost::shared_ptr<IOMsgReadComplete> msgRead) = 0;
		virtual void post(boost::shared_ptr<IOMsgWriteComplete> msgWrite) = 0;

	protected:

		boost::shared_ptr<ConnectionCtx> ctx;

	};


}

#endif
