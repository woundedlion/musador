#ifndef HTTP_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define HTTP_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include "IO/SocketListener.h"

namespace Musador
{
    class HTTPListener : public IO::SocketListener
    {
    public:

        /// @brief Constructor
        /// @param[in] localEP The local address to which the Listener will bind
        HTTPListener(const sockaddr_in& localEP);

        /// @brief Factory method for producing a Connection object accepted by this Listener.
        boost::shared_ptr<IO::Connection> createConnection();
    };
}

#endif