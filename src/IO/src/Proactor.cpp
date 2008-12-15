#include "Proactor.h"
#include "boost/bind.hpp"
#include "PipeListener.h"
#include "PipeConnection.h"
#include "SocketListener.h"
#include "SocketConnection.h"
#include <cassert>

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;
using namespace Musador::IO;

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

                // Look up completion context in internal job list
                boost::shared_ptr<CompletionCtx> ctx = this->releaseJob(ctxPtr);

                switch(ctx->msg->getType())
                {
                case MSG_SOCKET_ACCEPT_COMPLETE:
                    this->completeSocketAccept(ctx, nBytes);
                    break;
                case MSG_PIPE_ACCEPT_COMPLETE:
                    this->completePipeAccept(ctx);
                    break;
                case MSG_SOCKET_CONNECT_COMPLETE:
                    break;
                case MSG_PIPE_CONNECT_COMPLETE:
                    this->completePipeConnect(ctx);
                    break;
                case MSG_READ_COMPLETE:
                    this->completeRead(ctx, nBytes);
                    break;
                case MSG_WRITE_COMPLETE:
                    this->completeWrite(ctx, nBytes);
                    break;
                case MSG_ERROR:
                    // notify the handler
                    if (NULL != ctx->handler)
                    {
                        ctx->handler(ctx->msg, ctx->tag);
                    }
                    break;
                }
            }
            else 
            {
                DWORD err = ::GetLastError();
                if (WAIT_TIMEOUT != err)
                {
                    // Look up completion context in internal job list
                    boost::shared_ptr<CompletionCtx> ctx = this->releaseJob(ctxPtr);

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
                        boost::shared_ptr<MsgError> msgErr(new MsgError());
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
    boost::shared_ptr<MsgSocketAcceptComplete> msgAccept(new MsgSocketAcceptComplete());
    msgAccept->listener = listener;
    boost::shared_ptr<Connection> conn(listener->createConnection());
    if (NULL == conn) 
    {
        // Socket creation failed
        return;
    }
    msgAccept->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgAccept;
    ctx->handler = handler;
    ctx->tag = tag;

    // Make the async accept request
    DWORD nBytes = 0;
    this->addJob(ctx.get(),ctx);
    if ( !this->fnAcceptEx(	listener->getSocket(),
        boost::static_pointer_cast<SocketConnection>(conn)->getSocket(), 
        msgAccept->buf.begin(), 
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

            // Schedule notification
            boost::shared_ptr<MsgError> msgErr(new MsgError());
            msgErr->conn = conn;
            msgErr->err = err;
            ctx->msg = msgErr;
            ::PostQueuedCompletionStatus(this->iocp, 0, static_cast<ULONG_PTR>(listener->getSocket()), ctx.get());	

            return;
        }
    }
}

void 
Proactor::beginAccept(boost::shared_ptr<PipeListener> listener, 
                      EventHandler handler, 
                      boost::any tag /* = NULL */)
{
    // Create a listening pipe
    HANDLE hPipe = ::CreateNamedPipe(listener->getName().c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 
        PIPE_UNLIMITED_INSTANCES,
        4096,
        4096,
        INFINITE,
        NULL
        );

    if (INVALID_HANDLE_VALUE == hPipe)
    {
        LOG(Error) << "CreateNamedPipe() failed for " << listener->getName() <<": " << ::GetLastError();
        return;
    }

    // Associate the pipe handle with the IO completion port
    ::CreateIoCompletionPort(hPipe, this->iocp, reinterpret_cast<ULONG_PTR>(hPipe), NULL);

    // Create the completion data
    boost::shared_ptr<MsgPipeAcceptComplete> msgAccept(new MsgPipeAcceptComplete());
    msgAccept->listener = listener;
    boost::shared_ptr<PipeConnection> conn(boost::shared_static_cast<PipeConnection>(listener->createConnection()));
    conn->setPipe(hPipe);
    msgAccept->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgAccept;
    ctx->handler = handler;
    ctx->tag = tag;

    // Make the async accept request
    this->addJob(ctx.get(),ctx);
    if (!::ConnectNamedPipe(hPipe,ctx.get()))
    {
        DWORD err = ::GetLastError();
        if (ERROR_PIPE_CONNECTED == err)
        {
            // A completion packet is never queued to the port in this case
            // so send one ourselves to complete asynchronously
            ::PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(hPipe), ctx.get());	
        }
        else if (ERROR_IO_PENDING != err)
        {
            LOG(Error) << "ConnectNamedPipe() failed: " << err;

            // Schedule notification
            boost::shared_ptr<MsgError> msgErr(new MsgError());
            msgErr->conn = conn;
            msgErr->err = err;
            ctx->msg = msgErr;
            ::PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(hPipe), ctx.get());	

            return;
        }
    }
}


void 
Proactor::completeSocketAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
    // Retrieve the addresses and any received data and notify the event handler
    sockaddr_in * localAddr;
    sockaddr_in * remoteAddr;
    int localAddrSize = 0;
    int remoteAddrSize = 0;
    boost::shared_ptr<MsgSocketAcceptComplete> msgAccept(boost::shared_static_cast<MsgSocketAcceptComplete>(ctx->msg));
    this->fnGetAcceptExSockaddrs(msgAccept->buf.begin(), 
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
    msgAccept->buf.advanceEnd(nBytes);

    LOG(Debug) << "Accept completed: " << conn.toString();

    // Associate the socket with the IO completion port
    ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(conn.getSocket()), this->iocp, conn.getSocket(), NULL);

    // notify the handler
    if (NULL != ctx->handler)
    {
        ctx->handler(msgAccept,ctx->tag);
    }

    // notify the connection
    msgAccept->conn->onAcceptComplete(msgAccept, ctx->tag);
}

void 
Proactor::completePipeAccept(boost::shared_ptr<CompletionCtx> ctx)
{
    PipeConnection& conn = static_cast<PipeConnection&>(*ctx->msg->conn);
    LOG(Debug) << "Accept completed: " << conn.toString();

    boost::shared_ptr<MsgPipeAcceptComplete> msgAccept(boost::shared_static_cast<MsgPipeAcceptComplete>(ctx->msg));

    // notify the handler
    if (NULL != ctx->handler)
    {		
        ctx->handler(msgAccept,ctx->tag);
    }

    // notify the connection
    msgAccept->conn->onAcceptComplete(msgAccept, ctx->tag);
}

void
Proactor::beginConnect(boost::shared_ptr<PipeConnection> conn, 
                       EventHandler handler, 
                       const std::wstring& dest, 
                       boost::any tag /* = NULL */)
{
    boost::shared_ptr<MsgPipeConnectComplete> msgConnect(new MsgPipeConnectComplete());

    // Create completion context to send off into asynchronous land
    msgConnect->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgConnect;
    ctx->handler = handler;
    ctx->tag = tag;

    this->addJob(ctx.get(),ctx);

    // Make the faux async connect request
    HANDLE p  = ::CreateFile(conn->getName().c_str(),
        FILE_READ_DATA | FILE_WRITE_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
        );

    if (INVALID_HANDLE_VALUE == p)
    {
        DWORD err = ::GetLastError();
        LOG(Error) << "CreateFile() failed to open name pipe: " << conn->getName() << " [" << err << "]";

        // Schedule notification
        boost::shared_ptr<MsgError> msgErr(new MsgError());
        msgErr->conn = conn;
        msgErr->err = err;
        ctx->msg = msgErr;
    }

    // Connect will complete through completion port
    conn->setPipe(p);
    ::PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(p), ctx.get());	
}

void
Proactor::completePipeConnect(boost::shared_ptr<CompletionCtx> ctx)
{

    PipeConnection& conn = static_cast<PipeConnection&>(*ctx->msg->conn);
    LOG(Debug) << "Connect completed: " << conn.toString();

    // Associate the socket with the IO completion port
    ::CreateIoCompletionPort(conn.getPipe(), this->iocp, reinterpret_cast<ULONG_PTR>(conn.getPipe()), NULL);

    boost::shared_ptr<MsgPipeConnectComplete> msgConnect(boost::shared_static_cast<MsgPipeConnectComplete>(ctx->msg));

    // notify the handler
    if (NULL != ctx->handler)
    {
        ctx->handler(msgConnect, ctx->tag);
    }
}

void 
Proactor::beginRead(boost::shared_ptr<SocketConnection> conn, 
                    EventHandler handler, 
                    boost::shared_ptr<MsgReadComplete> msgRead, 
                    boost::any tag /* = NULL */)
{
    if (msgRead->buf.numFree() == 0)
    {
        throw IOException() << "Message overflow at " << msgRead->MAX << " bytes";
    }

    // Create completion context to send off into asynchronous land
    msgRead->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgRead;
    ctx->handler = handler;
    ctx->tag = tag;

    // Make the async read request
    DWORD nBytes = 0;
    DWORD flags = 0;
    WSABUF buf = {0};
    buf.buf = msgRead->buf.end();
    buf.len = msgRead->buf.numFree();

    this->addJob(ctx.get(),ctx);

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

            // Schedule notification
            boost::shared_ptr<MsgError> msgErr(new MsgError());
            msgErr->conn = conn;
            msgErr->err = err;
            ctx->msg = msgErr;
            ::PostQueuedCompletionStatus(this->iocp, 0, static_cast<ULONG_PTR>(conn->getSocket()), ctx.get());	

            return;
        }
    }
}


void 
Proactor::beginRead(boost::shared_ptr<PipeConnection> conn, 
                    EventHandler handler, 
                    boost::shared_ptr<MsgReadComplete> msgRead, 
                    boost::any tag /* = NULL */)
{
    if (msgRead->buf.numFree() == 0)
    {
        throw IOException() << "Message overflow at " << msgRead->MAX << " bytes";
    }

    // Create completion context to send off into asynchronous land
    msgRead->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgRead;
    ctx->handler = handler;
    ctx->tag = tag;

    this->addJob(ctx.get(),ctx);

    DWORD nBytes = 0;
    if (0 == ::ReadFile(conn->getPipe(), 
        msgRead->buf.end(), 
        msgRead->buf.numFree(), 
        &nBytes,
        ctx.get()))
    {
        DWORD err = ::GetLastError();
        if (ERROR_IO_PENDING != err)
        {
            LOG(Error) << "ReadFile() failed on pipe " << conn->getPipe() << " [" << err << "]";

            // Schedule notification
            boost::shared_ptr<MsgError> msgErr(new MsgError());
            msgErr->conn = conn;
            msgErr->err = err;
            ctx->msg = msgErr;
            ::PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(conn->getPipe()), ctx.get());	

            return;
        }
    }
}

void 
Proactor::completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
    boost::shared_ptr<MsgReadComplete> msgRead(boost::shared_static_cast<MsgReadComplete>(ctx->msg));
    msgRead->buf.advanceEnd(nBytes);

    if (0 == nBytes)
    {
        // 0 bytes received signals connection error
        // notify the handler
        if (NULL != ctx->handler)
        {
            boost::shared_ptr<MsgError> msgErr(new MsgError());
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
                     boost::shared_ptr<MsgWriteComplete> msgWrite, 
                     boost::any tag /* = NULL */)
{
    // Create completion context to send off into asynchronous land
    msgWrite->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgWrite;
    ctx->handler = handler;
    ctx->tag = tag;

    // Make the async write request
    DWORD nBytes = 0;
    WSABUF buf = {0};
    buf.buf = msgWrite->buf.begin();
    buf.len = msgWrite->buf.numUsed();

    this->addJob(ctx.get(),ctx);

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

            // Schedule notification
            boost::shared_ptr<MsgError> msgErr(new MsgError());
            msgErr->conn = conn;
            msgErr->err = err;
            ctx->msg = msgErr;
            ::PostQueuedCompletionStatus(this->iocp, 0, static_cast<ULONG_PTR>(conn->getSocket()), ctx.get());	

            return;
        }
    }
}

void 
Proactor::beginWrite(boost::shared_ptr<PipeConnection> conn, 
                     EventHandler handler, 
                     boost::shared_ptr<MsgWriteComplete> msgWrite, 
                     boost::any tag /* = NULL */)
{
    // Create completion context to send off into asynchronous land
    msgWrite->conn = conn;
    boost::shared_ptr<CompletionCtx> ctx(new CompletionCtx());
    ::memset(ctx.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    ctx->msg = msgWrite;
    ctx->handler = handler;
    ctx->tag = tag;

    this->addJob(ctx.get(),ctx);

    // Make the async write request	
    DWORD nBytes = 0;
    if (0 == ::WriteFile(conn->getPipe(), 
        msgWrite->buf.begin(), 
        msgWrite->buf.numUsed(), 
        &nBytes,
        ctx.get()))
    {
        DWORD err = ::GetLastError();
        if (ERROR_IO_PENDING != err)
        {
            LOG(Error) << "WriteFile() failed on " << conn->toString() << " [" << err << "]";

            // Schedule notification
            boost::shared_ptr<MsgError> msgErr(new MsgError());
            msgErr->conn = conn;
            msgErr->err = err;
            ctx->msg = msgErr;
            ::PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(conn->getPipe()), ctx.get());	

            return;
        }
    }
}

void 
Proactor::completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes)
{
    boost::shared_ptr<MsgWriteComplete> msgWrite(boost::shared_static_cast<MsgWriteComplete>(ctx->msg));
    msgWrite->buf.advanceBegin(nBytes);

    LOG(Debug)	<< "Write completed: " << nBytes << " bytes from " << msgWrite->conn->toString();

    if (msgWrite->buf.numUsed() > 0)
    {
        // Not done writing, so reschedule the write with a double-dispatch
        msgWrite->conn->beginWrite(msgWrite, ctx->tag);
        return;
    }

    // notify the handler
    if (NULL != ctx->handler)
    {
        ctx->handler(msgWrite,ctx->tag);
    }
}

void 
Proactor::start(int numWorkers /* = 0 */)
{
    if (numWorkers == 0)
    {
        numWorkers = 2 * boost::thread::hardware_concurrency();
    }
    for (int i = 0; i < numWorkers; ++i)
    {
        this->workers.push_back(new boost::thread(boost::bind(&Proactor::runIO,this)));
    }
}

void 
Proactor::stop()
{
    for (unsigned int i = 0; i < this->workers.size(); ++i)
    {
        ::PostQueuedCompletionStatus(this->iocp,0,0,NULL);
    }

    for (std::vector<boost::thread *>::iterator iter = this->workers.begin(); iter != this->workers.end(); ++iter)
    {
        (*iter)->join();
        delete *iter;
        *iter = NULL;
    }
}

#endif

void
Proactor::addJob(CompletionCtx * key, boost::shared_ptr<CompletionCtx> job)
{
    Guard lock(this->jobsMutex);
    this->jobs[key] = job;
}

boost::shared_ptr<CompletionCtx>
Proactor::releaseJob(CompletionCtx * key)
{
    Guard lock(this->jobsMutex);
    JobCollection::iterator iter = this->jobs.find(key);
    assert(iter != this->jobs.end());
    boost::shared_ptr<CompletionCtx> job = iter->second;
    this->jobs.erase(iter);
    return job;
}