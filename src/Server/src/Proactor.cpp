#include "Proactor.h"
#include "boost/bind.hpp"
#include "PipeListener.h"
#include "PipeConnection.h"
#include "SocketListener.h"
#include "SocketConnection.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

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
		while (!this->doRecycle) 
		{
			DWORD nBytes = 0;
			CompletionCtx * ctxPtr = 0;
			ULONG_PTR completionKey;
			if (::GetQueuedCompletionStatus(this->iocp, &nBytes, &completionKey, reinterpret_cast<OVERLAPPED **>(&ctxPtr),500))
			{
				if (0 == completionKey)
				{
					break;
				}

				boost::shared_ptr<CompletionCtx> ctx(ctxPtr);
				switch(ctx->msg->getType())
				{
					case IO_SOCKET_ACCEPT_COMPLETE:
						this->completeSocketAccept(ctx, nBytes);
						break;
					case IO_PIPE_ACCEPT_COMPLETE:
						this->completePipeAccept(ctx);
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
					// TODO: These can leak if they don't complete e.g. on shutdown
					// Refcount the Completion Context again
					std::auto_ptr<CompletionCtx> ctx(ctxPtr);

					DWORD flags = 0;
	//				::WSAGetOverlappedResult(ctx->msg->conn->getSocket(),ctxPtr,&nBytes,FALSE,&flags);
	//				DWORD err = ::WSAGetLastError();
					if (WSAECONNRESET != err && WSA_OPERATION_ABORTED != err)
					{
						LOG(Error) << "GetQueuedCompletionStatus() failed: " << err;
					}

					// notify the handler
					if (NULL != ctx->handler)
					{
						boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
						msgErr->conn = ctx->msg->conn;
						msgErr->err = err;
						ctx->handler(msgErr,ctx->tag);
					}
				}
			}
		} 
	} while(this->doRecycle);
}

void 
Proactor::beginAccept(boost::shared_ptr<SocketListener> listener,
					  EventHandler handler, 
					  boost::any tag /* = NULL */)
{
	// Load AcceptEx function
	if (NULL == this->fnAcceptEx)
	{
		GUID guid = WSAID_ACCEPTEX;
		DWORD nBytes = 0;
		DWORD err = ::WSAIoctl(	listener->getSocket(), 
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
		DWORD err = ::WSAIoctl(	listener->getSocket(), 
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
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listener->getSocket()), this->iocp, listener->getSocket(), NULL);

	// Create the completion data
	boost::shared_ptr<IOMsgSocketAcceptComplete> msgAccept(new IOMsgSocketAcceptComplete());
	msgAccept->listener = listener;
	boost::shared_ptr<Connection> conn(listener->createConnection());
	if (NULL == conn) 
	{
		// Socket creation failed
		return;
	}
	msgAccept->conn = conn;
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgAccept;
	ctx->handler = handler;
	ctx->tag = tag;

	// Make the async accept request
	DWORD nBytes = 0;
	if ( !this->fnAcceptEx(	listener->getSocket(),
							boost::static_pointer_cast<SocketConnection>(conn)->getSocket(), 
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
			if (WSAECONNRESET != err && 
				WSAECONNABORTED != err && 
				WSAEINVAL != err &&
				WSAENOTSOCK != err)
			{
				LOG(Error) << "AcceptEx() failed.: " << err;
			}

			// notify the handler
			if (NULL != ctx->handler)
			{
				boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
				msgErr->conn = msgAccept->conn;
				msgErr->err = err;
				ctx->handler(msgErr,ctx->tag);
			}

			return;
		}
	}

	// Accept will complete asynchronously
	ctx.release();
}

void 
Proactor::beginAccept(boost::shared_ptr<PipeListener> listener, 
					  EventHandler handler, 
					  boost::any tag /* = NULL */)
{
	// Associate the pipe handle with the IO completion port
	::CreateIoCompletionPort(listener->getPipe(), this->iocp, reinterpret_cast<ULONG_PTR>(listener->getPipe()), NULL);

	// Create the completion data
	boost::shared_ptr<IOMsgPipeAcceptComplete> msgAccept(new IOMsgPipeAcceptComplete());
	msgAccept->listener = listener;
	boost::shared_ptr<Connection> conn(listener->createConnection());
	if (NULL == conn) 
	{
		// Pipe creation failed
		return;
	}
	msgAccept->conn = conn;
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgAccept;
	ctx->handler = handler;
	ctx->tag = tag;

	// Make the async accept request
	if (!::ConnectNamedPipe(listener->getPipe(),ctx.get()))
	{
		DWORD err = ::GetLastError();
		if (ERROR_PIPE_CONNECTED != err)
		{
			LOG(Error) << "ConnectNamedPipe() failed: " << err;

			// notify the handler
			if (NULL != ctx->handler)
			{
				boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
				msgErr->conn = msgAccept->conn;
				msgErr->err = err;
				ctx->handler(msgErr,ctx->tag);
			}

			return;
		}
	}

	// Accept will complete asynchronously
	ctx.release();
}


void 
Proactor::completeSocketAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
	// Retrieve the addresses and any received data and notify the event handler
	sockaddr_in * localAddr;
	sockaddr_in * remoteAddr;
	int localAddrSize = 0;
	int remoteAddrSize = 0;
	boost::shared_ptr<IOMsgSocketAcceptComplete> msgAccept(boost::shared_static_cast<IOMsgSocketAcceptComplete>(ctx->msg));
	this->fnGetAcceptExSockaddrs(	msgAccept->buf.get(), 
									0,
									sizeof(sockaddr_in) + 16,
									sizeof(sockaddr_in) + 16, 
									(sockaddr **)&localAddr, 
									&localAddrSize,
									(sockaddr **)&remoteAddr, 
									&remoteAddrSize);

	// Augment the accept message
	SocketConnection& conn = static_cast<SocketConnection&>(*msgAccept->conn);
	conn.setLocalEP(*localAddr);
	conn.setRemoteEP(*remoteAddr);
	msgAccept->len = nBytes;

	LOG(Debug) << "Accept completed: " << conn.toString();

	// Associate the socket with the IO completion port
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(conn.getSocket()), this->iocp, conn.getSocket(), NULL);

	// notify the handler
	if (NULL != ctx->handler)
	{
		ctx->handler(msgAccept,ctx->tag);
	}
}

void 
Proactor::completePipeAccept(boost::shared_ptr<CompletionCtx> ctx)
{
	// notify the handler
	if (NULL != ctx->handler)
	{
		boost::shared_ptr<IOMsgPipeAcceptComplete> msgAccept(boost::shared_static_cast<IOMsgPipeAcceptComplete>(ctx->msg));
		ctx->handler(msgAccept,ctx->tag);
	}
}

void 
Proactor::beginRead(boost::shared_ptr<SocketConnection> conn, 
						 EventHandler handler, 
						 boost::shared_ptr<IOMsgReadComplete> msgRead, 
						 boost::any tag /* = NULL */)
{
	if (msgRead->MAX == msgRead->len)
	{
		throw IOException() << "Message overflow at " << msgRead->MAX << " bytes";
	}

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
	buf.len = msgRead->MAX - msgRead->len;
	if ( 0 != ::WSARecv(conn->getSocket(),
						&buf,
						1,
						&nBytes,
						&flags,
						ctx.get(),
						NULL)) 
	{
		DWORD err = ::WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			if (WSAECONNRESET != err)
			{
				LOG(Error) << "WSARecv() failed on socket " << conn->getSocket() << " [" << err << "]";
			}
		
			// notify the handler
			if (NULL != ctx->handler)
			{
				boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
				msgErr->conn = conn;
				msgErr->err = err;
				ctx->handler(msgErr,ctx->tag);
			}

			return;
		}
	}

	// Read will complete asynchronously
	ctx.release();
}


void 
Proactor::beginRead(boost::shared_ptr<PipeConnection> conn, 
					EventHandler handler, 
					boost::shared_ptr<IOMsgReadComplete> msgRead, 
					boost::any tag /* = NULL */)
{
	if (msgRead->MAX == msgRead->len)
	{
		throw IOException() << "Message overflow at " << msgRead->MAX << " bytes";
	}

	// Create completion context to send off into asynchronous land
	msgRead->conn = conn;
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgRead;
	ctx->handler = handler;
	ctx->tag = tag;

	if (0 != ::ReadFile(conn->getPipe(), 
						msgRead->buf.get() + msgRead->len, 
						msgRead->MAX - msgRead->len, 
						NULL,
						ctx.get()))
	{
		DWORD err = ::GetLastError();
		if (ERROR_IO_PENDING != err)
		{
			LOG(Error) << "ReadFile() failed on pipe " << conn->getPipe() << " [" << err << "]";

			// notify the handler
			if (NULL != ctx->handler)
			{
				boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
				msgErr->conn = conn;
				msgErr->err = err;
				ctx->handler(msgErr,ctx->tag);
			}

			return;
		}
	}

	// Read will complete asynchronously
	ctx.release();
}

void 
Proactor::completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
	boost::shared_ptr<IOMsgReadComplete> msgRead(boost::shared_static_cast<IOMsgReadComplete>(ctx->msg));
	msgRead->len += nBytes;

	if (0 == nBytes)
	{
		// 0 bytes received signals connection error
		// notify the handler
		if (NULL != ctx->handler)
		{
			boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
			msgErr->conn = msgRead->conn;
			msgErr->err = static_cast<int>(::WSAGetLastError());
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

void 
Proactor::beginWrite(boost::shared_ptr<SocketConnection> conn, 
						  EventHandler handler, 
						  boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
						  boost::any tag /* = NULL */)
{
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
		DWORD err = ::WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			if (WSAECONNRESET != err)
			{
				LOG(Error) << "WSASend() failed on " << conn->toString() << " [" << err << "]";
			}

			// notify the handler
			if (NULL != ctx->handler)
			{
				boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
				msgErr->conn = conn;
				msgErr->err = err;
				ctx->handler(msgErr,ctx->tag);
			}

			return;
		}
	}

	// Write will complete asynchronously
	ctx.release();
}

void 
Proactor::beginWrite(boost::shared_ptr<PipeConnection> conn, 
				     EventHandler handler, 
				     boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
				     boost::any tag /* = NULL */)
{
	// Create completion context to send off into asynchronous land
	msgWrite->conn = conn;
	std::auto_ptr<CompletionCtx> ctx(new CompletionCtx());
	::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
	ctx->msg = msgWrite;
	ctx->handler = handler;
	ctx->tag = tag;

	// Make the async write request
	if ( 0 != ::WriteFile(conn->getPipe(), 
						  msgWrite->buf.get() + msgWrite->off, 
						  msgWrite->len - msgWrite->off, 
						  NULL,ctx.get()))
	{
		DWORD err = ::GetLastError();
		if (ERROR_IO_PENDING != err)
		{
			LOG(Error) << "WriteFile() failed on " << conn->toString() << " [" << err << "]";
			
			// notify the handler
			if (NULL != ctx->handler)
			{
				boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
				msgErr->conn = conn;
				msgErr->err = err;
				ctx->handler(msgErr,ctx->tag);
			}

			return;
		}
	}

	// Write will complete asynchronously
	ctx.release();
}

void 
Proactor::completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
	boost::shared_ptr<IOMsgWriteComplete> msgWrite(boost::shared_static_cast<IOMsgWriteComplete>(ctx->msg));
	msgWrite->off += nBytes;
	if (msgWrite->off < msgWrite->len)
	{
		// Not done writing, so reschedule the write with a double-dispatch
		msgWrite->conn->beginWrite(msgWrite, ctx->tag);
		return;
	}

	LOG(Debug)	<< "Write completed: " << msgWrite->off << " bytes from " << msgWrite->conn->toString();

	// notify the handler
	if (NULL != ctx->handler)
	{
		ctx->handler(msgWrite,ctx->tag);
	}
}

void 
Proactor::start(int numWorkers /* = 0 */)
{
	for (int i = 0; i < numWorkers; ++i)
	{
		this->workers.push_back(new boost::thread(boost::bind(&Proactor::runIO,this)));
	}
}

void 
Proactor::stop()
{
	::PostQueuedCompletionStatus(this->iocp,0,0,NULL);
	for (std::vector<boost::thread *>::iterator iter = this->workers.begin(); iter != this->workers.end(); ++iter)
	{
		(*iter)->join();
		delete *iter;
		*iter = NULL;
	}
}

#endif