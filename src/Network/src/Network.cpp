#include "Network.h"

using namespace Musador;

Network::Network()
{
    // Initialize Winsock
    WSADATA wsaData;
    ::WSAStartup(MAKEWORD(2,0), &wsaData);
}

Network::~Network()
{
    ::WSACleanup();
}

int Network::getLastError()
{
    return ::WSAGetLastError();
}

SOCKET Network::socket(int af, int type, int protocol, bool async)
{
    SOCKET s = NULL;
    if (async)
    {
        s = ::WSASocket(af,type,protocol,NULL,NULL,WSA_FLAG_OVERLAPPED);
    }
    else
    {
        s = ::socket(af,type,protocol);
    }
    if (INVALID_SOCKET == s)
    {
        throw NetworkException() << "Unable to create socket! (" << this->getLastError() << ")";
    }
    return s;
}

void Network::bind(SOCKET s, sockaddr_in *localEP)
{
    int err = ::bind(s,(sockaddr *)localEP,sizeof(sockaddr_in));
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "Unable to bind socket " << s << "(" << this->getLastError() << ")"; 
    }
}

void Network::listen(SOCKET s, int backlog)
{
    if (0 == backlog)
    {
        backlog = SOMAXCONN;
    }
    int err = ::listen(s,backlog);
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "listen() failed on socket " << s << "(" << this->getLastError() << ")"; 
    }
}

SOCKET Network::accept(SOCKET s, sockaddr_in * clientSocket)
{
    int size = sizeof(sockaddr_in);
    SOCKET cs = ::accept(s, (sockaddr *)clientSocket, &size);
    if (SOCKET_ERROR == cs)
    {
        throw NetworkException() << "accept() failed on socket " << s << "(" << this->getLastError() << ")"; 
    }
    return cs;
}

void Network::connect(SOCKET s, sockaddr_in to)
{
    int size = sizeof(sockaddr_in);
    int err = ::connect(s, (sockaddr *)&to,sizeof(sockaddr_in));
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "connect() failed on socket " << s << "(" << this->getLastError() << ")"; 
    }
}

void Network::closeSocket(SOCKET s)
{
    int err = ::closesocket(s);
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "closesocket() failed on socket " << s << "(" << this->getLastError() << ")";	
    }
}

void Network::setBIO(SOCKET s, bool blocking)
{
    u_long nonblocking = blocking ? 0 : 1;
    int err = ::ioctlsocket(s,FIONBIO,&nonblocking);
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "Unable to set " << (blocking ? "blocking" : "non-blocking") << " I/O on socket " << s << "(" << this->getLastError() << ")"; 
    }
}

void Network::setBroadcast(SOCKET s, bool enableBroadcast)
{
    int valLen = sizeof(BOOL);
    BOOL val = true;
    int err = ::setsockopt(s,SOL_SOCKET,SO_BROADCAST,(char *)&val, valLen);
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "Unable to " << (enableBroadcast ? "enable" : "disable") << " broadcast on socket " << s << "(" << this->getLastError() << ")"; 
    }
}

int Network::recv(SOCKET s, char * buf, int len, int flags)
{
    int err = ::recv(s, buf, len,flags);
    if (SOCKET_ERROR == err)
    {
        int errlen = sizeof(err);
        ::getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
        if (err == WSAEWOULDBLOCK)
        {
            return 0;
        }
        else
        {
            throw NetworkException() << "recv() failed on socket " << s << " (" << this->getLastError() << ")";
        }
    }
    return err;
}

int Network::send(SOCKET s, const char * buf,int len, int flags /* = NULL */)
{
    int err = ::send(s, buf, len,flags);
    if (SOCKET_ERROR == err)
    {
        err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
            return 0;
        }
        else
        {
            throw NetworkException() << "send() failed on socket " << s << " (" << err << ")";
        }
    }
    return err;
}

int Network::select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, const struct timeval * timeout)
{
    int err = ::select(nfds,readfds,writefds,errorfds,timeout);
    if (SOCKET_ERROR == err)
    {
        throw NetworkException() << "select() failed (" << this->getLastError() << ")";
    }
    return err;
}

bool Network::fdIsSet(SOCKET s, fd_set * fds)
{
    return (FD_ISSET(s,fds) ? true : false);
}

void Network::fdSet(SOCKET s, fd_set * fds)
{
    FD_SET(s,fds);
}

void Network::fdClr(SOCKET s, fd_set * fds)
{
    FD_CLR(s,fds);
}

void Network::fdZero(fd_set * fds)
{
    FD_ZERO(fds);
}