#ifndef NETWORK_D0D77706_18C0_4a05_BBAD_41A7F8B529E7
#define NETWORK_D0D77706_18C0_4a05_BBAD_41A7F8B529E7

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#endif

#include "Utilities/Singleton.h"
#include "Utilities/StreamException.h"

namespace Musador
{

    class Network : public Util::Singleton<Network>
    {
        friend class Util::Singleton<Network>;
    public:

        ~Network();

        SOCKET socket(int af, int type, int protocol, bool async = true);

        void bind(SOCKET s,sockaddr_in * localEP);

        void listen(SOCKET s, int backlog = 0);

        SOCKET accept(SOCKET s, sockaddr_in * from);

        void connect(SOCKET s, sockaddr_in to);

        void closeSocket(SOCKET s);

        void setBIO(SOCKET s, bool blocking);

        void setBroadcast(SOCKET s, bool enableBroadcast);

        int recv(SOCKET s, char * buf, int len, int flags = NULL);

        int send(SOCKET s, const char * buf,int len, int flags = NULL);

        int select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, const struct timeval * timeout);

        bool fdIsSet(SOCKET s, fd_set * fds);

        void fdSet(SOCKET s, fd_set * fds);

        void fdClr(SOCKET s, fd_set * fds);

        void fdZero(fd_set * fds);

        int getLastError();

        size_t getRecvBufferSize(SOCKET s);

        void setRecvBufferSize(SOCKET s, size_t size);

        size_t getSendBufferSize(SOCKET s);

        void setSendBufferSize(SOCKET s, size_t size);

    private:

        Network();

    };
}

class NetworkException : public Util::StreamException<NetworkException>
{
public:
    NetworkException() {}
    ~NetworkException() {}
};

#endif