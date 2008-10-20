#ifndef SOCKET_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define SOCKET_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Listener.h"
#include "Network/Network.h"

namespace Musador
{
    class ConnectionCtx;

    class SocketListener : public Listener, public boost::enable_shared_from_this<SocketListener>
    {
    public:

        SocketListener(const sockaddr_in& localEP, 
            int socketType = SOCK_STREAM, 
            int socketProto = IPPROTO_TCP);

        ~SocketListener();

        void beginAccept(EventHandler handler, boost::any tag = NULL);

        void close();

        SOCKET getSocket();

    private:

        SOCKET sock;
        sockaddr_in localEP;

    };

    inline SOCKET SocketListener::getSocket()
    {
        return this->sock;
    }
}


#endif