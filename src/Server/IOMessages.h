#ifndef IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D
#define IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D

#include <assert.h>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include "Network/Network.h"

namespace Musador
{

	class Connection;

	const int SEND_CHUNK_SIZE = 4096;
	const int RECV_CHUNK_SIZE = 4096;

	enum IOMsgType
	{
		IO_WRITE_COMPLETE,
		IO_READ_COMPLETE,
		IO_ACCEPT_COMPLETE,
		IO_ERROR
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

	class IOMsgWriteComplete : public IOMsg
	{
	public:

		inline IOMsgWriteComplete() : IOMsg(IO_WRITE_COMPLETE),
			buf(new char[SEND_CHUNK_SIZE]),
			len(0),
			off(0)
		{
		}

		boost::shared_array<char> buf;
		unsigned long len;
		unsigned long off;
	};

	class IOMsgReadComplete : public IOMsg
	{
	public:

		inline IOMsgReadComplete() : IOMsg(IO_READ_COMPLETE),
			buf(new char[RECV_CHUNK_SIZE]),
			len(0),
			off(0)
		{
		}

		boost::shared_array<char> buf;
		unsigned long len;
		unsigned long off;
	};

	class IOMsgAcceptComplete : public IOMsg
	{
	public:

		inline IOMsgAcceptComplete() : IOMsg(IO_ACCEPT_COMPLETE),
			buf(new char[2 * (sizeof(sockaddr_in) + 16)]),
			len(0),
			off(0)
		{
		}

		SOCKET listener;
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

	};

}
#endif