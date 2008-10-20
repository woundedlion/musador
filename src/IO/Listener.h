#ifndef LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include "IOMessages.h"

namespace Musador
{
    class Connection;
    class ConnectionCtx;

    class Listener
    {
    public:

        virtual ~Listener() {}

        virtual void beginAccept(EventHandler handler, boost::any tag = NULL) = 0;

        virtual void close() = 0;

        virtual boost::shared_ptr<Connection> createConnection() = 0;

    };
}


#endif