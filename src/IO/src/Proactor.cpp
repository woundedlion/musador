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
fnGetAcceptExSockaddrs(NULL),
fnAcceptEx(NULL),
doShutdown(false),
doRecycle(false)
{
    this->iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,NULL);
}

Proactor::~Proactor()
{
    if (NULL != this->iocp)
    {
        ::CloseHandle(this->iocp);
    }
}

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
            Job * ctxPtr = 0;
            ULONG_PTR completionKey;
            if (::GetQueuedCompletionStatus(this->iocp, &nBytes, &completionKey, reinterpret_cast<OVERLAPPED **>(&ctxPtr),500))
            {
                if (0 == completionKey)
                {
                    LOG(Info) << "Event thread " << ::GetCurrentThreadId() << " exiting";
                    break;
                }

                // Look up job in internal job list
                boost::shared_ptr<Job> job = this->releaseJob(ctxPtr);

                switch(job->msg->getType())
                {
                case MSG_SOCKET_ACCEPT_COMPLETE:
                    this->completeSocketAccept(job, nBytes);
                    break;
                case MSG_PIPE_ACCEPT_COMPLETE:
                    this->completePipeAccept(job);
                    break;
                case MSG_SOCKET_CONNECT_COMPLETE:
                    break;
                case MSG_PIPE_CONNECT_COMPLETE:
                    this->completePipeConnect(job);
                    break;
                case MSG_READ_COMPLETE:
                    this->completeRead(job, nBytes);
                    break;
                case MSG_WRITE_COMPLETE:
                    this->completeWrite(job, nBytes);
                    break;
                case MSG_NOTIFY:
                    this->completeNotify(job);
                }
            }
            else 
            {
                DWORD err = ::GetLastError();
                if (WAIT_TIMEOUT != err)
                {
                    if (ctxPtr == NULL) {
                        LOG(Error) << "GetQueuedCompletionStatus() failed: " << err;
                    } 
                    else 
                    {
                        // Look up job in internal job list
                        boost::shared_ptr<Job> job = this->releaseJob(ctxPtr);

                        if (NULL != job->handler)
                        {
                            // Set the appropriate error and notify the handler
                            job->msg->setError(err);
                            job->handler(job->msg,job->tag);
                        }
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

    // Create the completion data
    boost::shared_ptr<MsgSocketAcceptComplete> msgAccept(new MsgSocketAcceptComplete());
    msgAccept->listener = listener;
    boost::shared_ptr<Connection> conn(listener->createConnection());
    msgAccept->conn = conn;
    boost::shared_ptr<Job> job(new Job());
    job->msg = msgAccept;
    job->handler = handler;
    job->tag = tag;

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
            err = ::WSAGetLastError();
            LOG(Critical) << "Could not load AcceptEx function [" << err << "]";
            this->fnAcceptEx = NULL;

            // Schedule error notification
            job->msg->setError(err);
            this->postJob(job);
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
            err = ::WSAGetLastError();
            LOG(Critical) << "Could not load GetAcceptExSockaddrs function [" << err << "]";
            this->fnGetAcceptExSockaddrs = NULL;

            // Schedule error notification
            job->msg->setError(err);
            this->postJob(job);
            return;
        }
    }

    // Associate the listening socket with the IO completion port
    ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listener->getSocket()), this->iocp, listener->getSocket(), NULL);

    // Make the async accept request
    DWORD nBytes = 0;
    this->addJob(job);
    if ( !this->fnAcceptEx(	listener->getSocket(),
        boost::static_pointer_cast<SocketConnection>(conn)->getSocket(), 
        msgAccept->buf.begin(), 
        0, 
        sizeof(sockaddr_in) + 16, 
        sizeof(sockaddr_in) + 16, 
        &nBytes, 
        job.get()))
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

            // Schedule error notification
            job->msg->setError(err);
            this->postJob(job);	
        }
    }
    return;
}

void
Proactor::beginAccept(boost::shared_ptr<PipeListener> listener, 
                      EventHandler handler, 
                      boost::any tag /* = NULL */)
{
    // Create the completion data
    boost::shared_ptr<MsgPipeAcceptComplete> msgAccept(new MsgPipeAcceptComplete());
    msgAccept->listener = listener;
    boost::shared_ptr<PipeConnection> conn(boost::shared_static_cast<PipeConnection>(listener->createConnection()));
    msgAccept->conn = conn;
    boost::shared_ptr<Job> job(new Job());
    ::memset(job.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    job->msg = msgAccept;
    job->handler = handler;
    job->tag = tag;

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
        DWORD err = ::GetLastError();
        LOG(Error) << "CreateNamedPipe() failed for " << listener->getName() <<": " << err;

        // Schedule error notification
        job->msg->setError(err);
        this->postJob(job);
        return;
    }

    conn->setPipe(hPipe);

    // Associate the pipe handle with the IO completion port
    ::CreateIoCompletionPort(hPipe, this->iocp, reinterpret_cast<ULONG_PTR>(hPipe), NULL);

    // Make the async accept request
    this->addJob(job);
    if (!::ConnectNamedPipe(hPipe,job.get()))
    {
        DWORD err = ::GetLastError();
        if (ERROR_IO_PENDING != err)
        {
            LOG(Error) << "ConnectNamedPipe() failed: " << err;

            // Schedule error notification
            job->msg->setError(err);
            this->postJob(job);
        }
    }
    return;
}


void 
Proactor::completeSocketAccept(boost::shared_ptr<Job> job, unsigned long nBytes)
{
    // Retrieve the addresses and any received data and notify the event handler
    sockaddr_in * localAddr;
    sockaddr_in * remoteAddr;
    int localAddrSize = 0;
    int remoteAddrSize = 0;
    boost::shared_ptr<MsgSocketAcceptComplete> msgAccept(boost::shared_static_cast<MsgSocketAcceptComplete>(job->msg));
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
    if (NULL != job->handler)
    {
        job->handler(msgAccept,job->tag);
    }

    // notify the connection
    msgAccept->conn->onAcceptComplete(msgAccept, job->tag);
}

void 
Proactor::completePipeAccept(boost::shared_ptr<Job> job)
{
    boost::shared_ptr<MsgPipeAcceptComplete> msgAccept(boost::shared_static_cast<MsgPipeAcceptComplete>(job->msg));
    LOG(Debug) << "Accept completed: " << msgAccept->conn->toString();

    // notify the handler
    if (NULL != job->handler)
    {		
        job->handler(msgAccept,job->tag);
    }

    // notify the connection
    msgAccept->conn->onAcceptComplete(msgAccept, job->tag);
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
    boost::shared_ptr<Job> job(new Job());
    ::memset(job.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    job->msg = msgConnect;
    job->handler = handler;
    job->tag = tag;

    this->addJob(job);

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
        job->msg->setError(err);
    }

    // Connect will complete through completion port
    conn->setPipe(p);
    this->postJob(job);

    return;
}

void
Proactor::completePipeConnect(boost::shared_ptr<Job> job)
{
    boost::shared_ptr<MsgPipeConnectComplete> msgConnect(boost::shared_static_cast<MsgPipeConnectComplete>(job->msg));
    PipeConnection& conn = static_cast<PipeConnection&>(*msgConnect->conn);
    LOG(Debug) << "Connect completed: " << conn.toString();

    // Associate the socket with the IO completion port
    ::CreateIoCompletionPort(conn.getPipe(), this->iocp, reinterpret_cast<ULONG_PTR>(conn.getPipe()), NULL);

    // notify the handler
    if (NULL != job->handler)
    {
        job->handler(msgConnect, job->tag);
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
    boost::shared_ptr<Job> job(new Job());
    ::memset(job.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    job->msg = msgRead;
    job->handler = handler;
    job->tag = tag;

    // Make the async read request
    DWORD nBytes = 0;
    DWORD flags = 0;
    WSABUF buf = {0};
    buf.buf = msgRead->buf.end();
    buf.len = msgRead->buf.numFree();

    this->addJob(job);

    if ( 0 != ::WSARecv(conn->getSocket(),
        &buf,
        1,
        &nBytes,
        &flags,
        job.get(),
        NULL)) 
    {
        DWORD err = ::WSAGetLastError();
        if (ERROR_IO_PENDING != err)
        {
            if (WSAECONNRESET != err)
            {
                LOG(Error) << "WSARecv() failed on socket " << conn->getSocket() << " [" << err << "]";
            }

            // Schedule error notification
            job->msg->setError(err);
            this->postJob(job);
        }
    }
    return;
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
    boost::shared_ptr<Job> job(new Job());
    ::memset(job.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    job->msg = msgRead;
    job->handler = handler;
    job->tag = tag;

    this->addJob(job);

    DWORD nBytes = 0;
    if (0 == ::ReadFile(conn->getPipe(), 
        msgRead->buf.end(), 
        msgRead->buf.numFree(), 
        &nBytes,
        job.get()))
    {
        DWORD err = ::GetLastError();
        if (ERROR_IO_PENDING != err)
        {
            LOG(Error) << "ReadFile() failed on pipe " << conn->getPipe() << " [" << err << "]";

            // Schedule notification
            job->msg->setError(err);
            this->postJob(job);	
        }
    }
    return;
}

void 
Proactor::completeRead(boost::shared_ptr<Job> job, unsigned long nBytes)
{
    boost::shared_ptr<MsgReadComplete> msgRead(boost::shared_static_cast<MsgReadComplete>(job->msg));
    msgRead->buf.advanceEnd(nBytes);

    if (0 == nBytes)
    {
        // 0 bytes received signals connection error
        // notify the handler
        if (NULL != job->handler)
        {
            DWORD err = ::GetLastError();
            job->msg->setError(err);
        }

        return;
    }

    LOG(Debug)	<< "Read completed: " << nBytes << " bytes from " << msgRead->conn->toString();

    // notify the handler
    if (NULL != job->handler)
    {
        job->handler(msgRead,job->tag);
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
    boost::shared_ptr<Job> job(new Job());
    ::memset(job.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    job->msg = msgWrite;
    job->handler = handler;
    job->tag = tag;

    // Make the async write request
    DWORD nBytes = 0;
    WSABUF buf = {0};
    buf.buf = msgWrite->buf.begin();
    buf.len = msgWrite->buf.numUsed();

    this->addJob(job);

    if ( 0 != ::WSASend(conn->getSocket(),
        &buf,
        1,
        &nBytes,
        0,
        job.get(),
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
            job->msg->setError(err);
            this->postJob(job);	
        }
    }
    return;
}

void
Proactor::beginWrite(boost::shared_ptr<PipeConnection> conn, 
                     EventHandler handler, 
                     boost::shared_ptr<MsgWriteComplete> msgWrite, 
                     boost::any tag /* = NULL */)
{
    // Create completion context to send off into asynchronous land
    msgWrite->conn = conn;
    boost::shared_ptr<Job> job(new Job());
    ::memset(job.get(), 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
    job->msg = msgWrite;
    job->handler = handler;
    job->tag = tag;

    this->addJob(job);

    // Make the async write request	
    DWORD nBytes = 0;
    if (0 == ::WriteFile(conn->getPipe(), 
        msgWrite->buf.begin(), 
        msgWrite->buf.numUsed(), 
        &nBytes,
        job.get()))
    {
        DWORD err = ::GetLastError();
        if (ERROR_IO_PENDING != err)
        {
            LOG(Error) << "WriteFile() failed on " << conn->toString() << " [" << err << "]";

            // Schedule notification
            job->msg->setError(err);
            this->postJob(job);	
        }
    }
    return;
}

void 
Proactor::completeWrite(boost::shared_ptr<Job> job, unsigned long nBytes)
{
    boost::shared_ptr<MsgWriteComplete> msgWrite(boost::shared_static_cast<MsgWriteComplete>(job->msg));
    msgWrite->buf.advanceBegin(nBytes);

    LOG(Debug)	<< "Write completed: " << nBytes << " bytes from " << msgWrite->conn->toString();

    if (msgWrite->buf.numUsed() > 0)
    {
        // Not done writing, so reschedule the write with a double-dispatch
        msgWrite->conn->beginWrite(msgWrite, job->tag);
        return;
    }

    // notify the handler
    if (NULL != job->handler)
    {
        job->handler(msgWrite,job->tag);
    }
}

void
Proactor::beginNotify(EventHandler handler, boost::shared_ptr<MsgNotify> msgNotify, boost::any tag /* = NULL */)
{
    boost::shared_ptr<Job> job(new Job());
    job->handler = handler;
    job->msg = msgNotify;
    job->tag = tag;
    this->addJob(job);
    this->postJob(job);	
    return;
}

void
Proactor::completeNotify(boost::shared_ptr<Job> job)
{
    LOG(Debug)	<< "Notify completed.";
    boost::shared_ptr<MsgNotify> msgNotify(boost::shared_static_cast<MsgNotify>(job->msg));
    // notify the handler
    assert(NULL != job->handler);
    job->handler(msgNotify,job->tag);
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

void
Proactor::addJob(boost::shared_ptr<Job> job)
{
    Guard lock(this->jobsMutex);
    this->jobs[job.get()] = job;
}

boost::shared_ptr<Proactor::Job>
Proactor::releaseJob(Job * key)
{
    Guard lock(this->jobsMutex);
    JobCollection::iterator iter = this->jobs.find(key);
    assert(iter != this->jobs.end());
    boost::shared_ptr<Job> job = iter->second;
    this->jobs.erase(iter);
    return job;
}

void
Proactor::postJob(boost::shared_ptr<Proactor::Job> job)
{
    ::PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(job.get()), job.get());	
}


//////////////////////////////////////////////////////////////////////////
// Job
//////////////////////////////////////////////////////////////////////////

Proactor::Job::Job() 
{
    ::memset(this, 0, sizeof(OVERLAPPED)); // clear OVERLAPPED part of structure
}
