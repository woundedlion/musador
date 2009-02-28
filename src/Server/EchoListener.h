#ifndef ECHO_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define ECHO_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include "IO/SocketListener.h"

namespace Musador
{
    /// @class EchoListener
    /// @brief Simple Echo Server Listener
    /// A Listener which accepts EchoConnection connections
    class EchoListener : public IO::SocketListener
    {
    public:

        /// @brief Constructor
        /// @param[in] localEP The local address to which the Listener will bind
        EchoListener(const sockaddr_in& localEP);

        /// @brief Factory method for producing a Connection object accepted by this Listener.
        boost::shared_ptr<IO::Connection> createConnection();
    };
}

#endif