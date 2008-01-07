#include "Proactor.h"
#include "boost/bind.hpp"

#include "Logger/Logger.h"
#define LOG_SENDER L"Proactor"

using namespace Musador;

Proactor::Proactor() :
#ifdef WIN32
fnGetAcceptExSockaddrs(NULL),
fnAcceptEx(NULL),
#endif
doShutdown(false),
doRecycle(false)
{
#ifdef WIN32
	this->iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,NULL);
#endif
}

Proactor::~Proactor()
{
	if (NULL != this->iocp)
	{
		::CloseHandle(this->iocp);
	}
}

#ifdef WIN32

void Proactor::runIO()
{	
	do {
		if (NULL == this->iocp)
		{
			LOG(Critical) << "Could not create I/O Completion Port: " << ::GetLastError();
		}

		this->doRecycle = false;
		while (!this->doShutdown && !this->doRecycle) 
		{
			DWORD nBytes = 0;
			CompletionCtx * ctxPtr = 0;
			ULONG_PTR completionKey;
			if (::GetQueuedCompletionStatus(this->iocp, &nBytes, &completionKey, reinterpret_cast<OVERLAPPED **>(&ctxPtr),500))
			{
				boost::shared_ptr<CompletionCtx> ctx(ctxPtr);
				switch(ctx->msg->getType())
				{
					case IO_ACCEPT_COMPLETE:
						this->completeAccept(ctx, nBytes);
						break;
					case IO_READ_COMPLETE:
						this->completeRead(ctx, nBytes);
						break;
					case IO_WRITE_COMPLETE:
						this->completeWrite(ctx, nBytes);
						break;
				}		
			}
			else 
			{
				DWORD err = ::GetLastError();
				if (WAIT_TIMEOUT != err)
				{
					LOG(Error) << "Could not dequeue completion packet: " << err;
				}
			}

		} // end while(!shutdwown)
	} while(this->doRecycle);
}

void Proactor::beginAccept(SOCKET listenSocket, EventHandler handler, boost::any tag /* = NULL */)
{
	boost::shared_ptr<IOMsgAcceptComplete> msgAccept(new IOMsgAcceptComplete());
	this->beginAccept(listenSocket, handler, msgAccept, tag);
}

void Proactor::beginAccept(SOCKET listenSocket, EventHandler handler, boost::shared_ptr<IOMsgAcceptComplete> msgAccept, boost::any tag /* = NULL */)
{
	// Load AcceptEx function
	if (NULL == this->fnAcceptEx)
	{
		GUID guid = WSAID_ACCEPTEX;
		DWORD nBytes = 0;
		DWORD err = ::WSAIoctl(	listenSocket, 
								SIO_GET_EXTENSION_FUNCTION_POINTER, 
								&guid, 
								sizeof(guid),
								&this->fnAcceptEx, 
								sizeof(this->fnAcceptEx), 
								&nBytes, 
								NULL, 
								NULL);
		if (SOCKET_ERROR == err )
		{
			LOG(Critical) << "Could not load AcceptEx function (" << ::WSAGetLastError() << ")";
			this->fnAcceptEx = NULL;
			return;
		}
	}
	// Load GetAcceptExSockaddrs function
	if (NULL == this->fnGetAcceptExSockaddrs)
	{
		GUID guid = WSAID_GETACCEPTEXSOCKADDRS;
		DWORD nBytes = 0;
		DWORD err = ::WSAIoctl(	listenSocket, 
								SIO_GET_EXTENSION_FUNCTION_POINTER, 
								&guid, 
								sizeof(guid),
								&this->fnGetAcceptExSockaddrs, 
								sizeof(this->fnGetAcceptExSockaddrs), 
								&nBytes, 
								NULL, 
								NULL);
		if (SOCKET_ERROR == err )
		{
			LOG(Critical) << "Could not load GetAcceptExSockaddrs function (" << ::WSAGetLastError() << ")";
			this->fnGetAcceptExSockaddrs = NULL;
			return;
		}
	}
	
	// Associate the listening socket with the IO completion port
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), this->iocp, NULL, NULL);
	msgAccept->listener = listenSocket;

	// Create completion context to send off into asynchronous land
	if (NULL == msgAccept->conn)
	{
		msgAccept->conn.reset(new Connection(Network::instance()->socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
	}
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgAccept;
	ctx->handler = handler;
	ctx->tag = tag;

	// Make the async accept request
	DWORD nBytes = 0;
	if ( !this->fnAcceptEx(	listenSocket,
							msgAccept->conn->getSocket(), 
							msgAccept->buf.get(), 
							0, 
							sizeof(sockaddr_in) + 16, 
							sizeof(sockaddr_in) + 16, 
							&nBytes, 
							ctx.get()))
	{
		DWORD err = ::WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			LOG(Critical) << "AcceptEx failed to accept a connection: " << err;
			return;
		}
	}

	// Accept will complete asynchronously
	ctx.release();
}

void Proactor::completeAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
	// Retrieve the addresses and any received data and notify the event handler
	sockaddr_in * localAddr;
	sockaddr_in * remoteAddr;
	int localAddrSize = 0;
	int remoteAddrSize = 0;
	boost::shared_ptr<IOMsgAcceptComplete> msgAccept(boost::shared_static_cast<IOMsgAcceptComplete>(ctx->msg));
	this->fnGetAcceptExSockaddrs(	msgAccept->buf.get(), 
									0,
									sizeof(sockaddr_in) + 16,
									sizeof(sockaddr_in) + 16, 
									(sockaddr **)&localAddr, 
									&localAddrSize,
									(sockaddr **)&remoteAddr, 
									&remoteAddrSize);

	// Augment the accept message
	msgAccept->conn->setLocalEP(*localAddr);
	msgAccept->conn->setRemoteEP(*remoteAddr);
	msgAccept->len = nBytes;

	LOG(Info) << "Connection accepted: " << msgAccept->conn->toString();

	// notify the handler
	if (NULL != ctx->handler)
	{
		ctx->handler(msgAccept,ctx->tag);
	}
}

void Proactor::beginRead(boost::shared_ptr<Connection> conn, EventHandler handler, boost::any tag /* = NULL */)
{
	boost::shared_ptr<IOMsgReadComplete> msgRead(new IOMsgReadComplete());
	this->beginRead(conn, handler, msgRead, tag);
}

void Proactor::beginRead(boost::shared_ptr<Connection> conn, 
						 EventHandler handler, 
						 boost::shared_ptr<IOMsgReadComplete> msgRead, 
						 boost::any tag /* = NULL */)
{
	// Associate the socket with the IO completion port
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(conn->getSocket()), this->iocp, NULL, NULL);

	// Create completion context to send off into asynchronous land
	msgRead->conn = conn;
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgRead;
	ctx->handler = handler;
	ctx->tag = tag;

	// Make the async read request
	DWORD nBytes = 0;
	DWORD flags = 0;
	WSABUF buf = {0};
	buf.buf = msgRead->buf.get() + msgRead->len;
	buf.len = RECV_CHUNK_SIZE - msgRead->len;
	if ( 0 != ::WSARecv(conn->getSocket(),
						&buf,
						1,
						&nBytes,
						&flags,
						ctx.get(),
						NULL)) 
	{
		DWORD err = ::GetLastError();
		if (ERROR_IO_PENDING != err)
		{
			LOG(Critical) << "ReadFile() failed to initiate an asynchronous read operation on the socket: " << err;
			return;
		}
	}

	// Read will complete asynchronously
	ctx.release();
}

void Proactor::completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
	boost::shared_ptr<IOMsgReadComplete> msgRead(boost::shared_static_cast<IOMsgReadComplete>(ctx->msg));
	msgRead->len += nBytes;

	if (0 == nBytes)
	{
		// 0 bytes received signals connection error
		boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
		msgErr->conn = ctx->msg->conn;
		msgErr->err = static_cast<int>(::WSAGetLastError());
		// notify the handler
		if (NULL != ctx->handler)
		{
			ctx->handler(msgErr,ctx->tag);
		}
		return;
	}

	LOG(Debug)	<< "Read completed: " << nBytes << " bytes from " << msgRead->conn->toString();

	// notify the handler
	if (NULL != ctx->handler)
	{
		ctx->handler(msgRead,ctx->tag);
	}
}

void Proactor::beginWrite(boost::shared_ptr<Connection> conn, 
						  EventHandler handler, 
						  boost::shared_array<char> data, 
						  int len, 
						  boost::any tag /* = NULL */)
{
	boost::shared_ptr<IOMsgWriteComplete> msgWrite(new IOMsgWriteComplete());
	msgWrite->buf = data;
	msgWrite->len = len;
	this->beginWrite(conn, handler, msgWrite, tag);
}

void Proactor::beginWrite(boost::shared_ptr<Connection> conn, 
						  EventHandler handler, 
						  boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
						  boost::any tag /* = NULL */)
{
	// Associate the socket with the IO completion port
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(conn->getSocket()), this->iocp, NULL, NULL);

	// Create completion context to send off into asynchronous land
	msgWrite->conn = conn;
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgWrite;
	ctx->handler = handler;
	ctx->tag = tag;

	// Make the async write request
	DWORD nBytes = 0;
	WSABUF buf = {0};
	buf.buf = msgWrite->buf.get() + msgWrite->off;
	buf.len = msgWrite->len - msgWrite->off;
	if ( 0 != ::WSASend(conn->getSocket(),
						&buf,
						1,
						&nBytes,
						0,
						ctx.get(),
						NULL))
	{
		DWORD err = ::GetLastError();
		if (ERROR_IO_PENDING != err)
		{
			LOG(Critical) << "WriteFile() failed to initiate an asynchronous write operation on the socket: " << err;
			return;
		}
	}

	// Write will complete asynchronously
	ctx.release();
}

void Proactor::completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
	boost::shared_ptr<IOMsgWriteComplete> msgWrite(boost::shared_static_cast<IOMsgWriteComplete>(ctx->msg));
	msgWrite->off += nBytes;
	if (msgWrite-> off < msgWrite->len)
	{
		// Not done writing, so reschedule the write
		this->beginWrite(ctx->msg->conn, ctx->handler, msgWrite, ctx->tag);
		return;
	}

	LOG(Debug)	<< "Write completed: " << msgWrite->off << " bytes from " << msgWrite->conn->toString();

	// notify the handler
	if (NULL != ctx->handler)
	{
		ctx->handler(msgWrite,ctx->tag);
	}
}

#endif