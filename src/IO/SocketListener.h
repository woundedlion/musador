#ifndef SOCKET_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define SOCKET_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Listener.h"
#include "Network/Network.h"

namespace Musador
{
    namespace IO
    {
        class ConnectionCtx;

        /// @class SocketListener
        /// @brief Abstract base class for Socket Listeners.
        /// Provides an asynchronous interface to listen for incoming Socket connections.
        /// Specific protocol implementations, e.g. HTTPListener and EchoListener are derived from this class 
        /// and implement the createConnection factory method to produce the correct SocketConnection type
        /// on a successful ACCEPT.
        class SocketListener : public Listener, public boost::enable_shared_from_this<SocketListener>
        {
        public:

            /// @brief Constructor.
            /// @param[in] localEP The local IP endpoint on which to listen for incoming connections.
            /// @param[in] socketType The numeric socket type specifier, e.g. SOCK_STREAM, SOCK_DGRAM
            /// @param[in] socketProto The numeric protocol specifier, e.g. IPPROTO_TCP, IPPROTO_UDP
            SocketListener(const sockaddr_in& localEP, 
                int socketType = SOCK_STREAM, 
                int socketProto = IPPROTO_TCP);

            /// @brief Destructor.
            ~SocketListener();

            /// @brief Begin an asynchronous ACCEPT with this SocketListener.
            /// One incoming SocketConnection can be accepted for each outstanding invocation of this method.
            /// @param[in] handler EventHandler invoked when a Connection is accepted or the ACCEPT fails.
            /// @param[in] tag User-defined data passed along the handler upon completion of this request.
            void beginAccept(EventHandler handler, boost::any tag = NULL);

            /// @brief Close this SocketListener.
            /// Any outstanding ACCEPTS on this SocketListener will complete with an error.
            void close();

            /// @brief Get the underlying Socket belonging to this SocketListener.
            /// @returns A system-specific handle to the underlying Socket.
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
}

#endif