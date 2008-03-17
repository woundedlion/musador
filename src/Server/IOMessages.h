#ifndef IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D
#define IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D

#include <assert.h>
#include <boost/function.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include "Network/Network.h"

namespace Musador
{
	class Connection;
	class Listener;

	enum IOMsgType
	{
		IO_WRITE_COMPLETE,
		IO_READ_COMPLETE,
		IO_SOCKET_ACCEPT_COMPLETE,
		IO_PIPE_ACCEPT_COMPLETE,
		IO_ERROR,
		IO_SHUTDOWN
	};

	class IOMsg
	{
	public:

		IOMsg(IOMsgType type) : type(type) {}

		inline IOMsgType getType() { return type; }

		boost::shared_ptr<Connection> conn;

	private: 

		IOMsgType type;
	};

	class IOMsgShutdown : public IOMsg
	{
	public:
		inline IOMsgShutdown() : IOMsg(IO_SHUTDOWN) {}
	};

	class IOMsgWriteComplete : public IOMsg
	{
	public:

		inline IOMsgWriteComplete() : IOMsg(IO_WRITE_COMPLETE),
			buf(new char[IOMsgWriteComplete::MAX]),
			len(0),
			off(0)
		{
		}

		boost::shared_array<char> buf;
		unsigned long len;
		unsigned long off;
		static const int MAX = 4096;
	};

	class IOMsgReadComplete : public IOMsg
	{
	public:

		inline IOMsgReadComplete() : IOMsg(IO_READ_COMPLETE),
			buf(new char[IOMsgReadComplete::MAX]),
			len(0),
			off(0)
		{
		}

		boost::shared_array<char> buf;
		unsigned long len;
		unsigned long off;
		static const int MAX = 4096;
	};

	class IOMsgSocketAcceptComplete : public IOMsg
	{
	public:

		inline IOMsgSocketAcceptComplete() : IOMsg(IO_SOCKET_ACCEPT_COMPLETE),
			buf(new char[2 * (sizeof(sockaddr_in) + 16)]),
			len(0),
			off(0)
		{
		}

		boost::shared_ptr<Listener> listener;
		boost::shared_array<char> buf;
		unsigned long len;
		unsigned long off;
	};

	class IOMsgError : public IOMsg
	{
	public:

		inline IOMsgError() : IOMsg(IO_ERROR),
		err(0)
		{
		}

		int err;
		boost::any tag;
		
	};

	typedef boost::function2<void, boost::shared_ptr<IOMsg>, boost::any> EventHandler;

}

#include "Connection.h"

#endif