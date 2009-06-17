#ifndef LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include "IOMessages.h"

namespace Musador
{
    namespace IO
    {
        class Connection;
        class ConnectionCtx;

        /// @class Listener
        /// @brief Abstract base class for all Listeners.
        /// Provides an asynchronous interface to listen for various types of incoming connections.
        /// In general, concrete listener types, e.g. SocketListener and PipeListener derive
        /// from this class and implement the beginAccept and close methods.
        /// Specific protocol implementations, e.g. HTTPListener and EchoListener 
        /// are then derived from the concrete listener types
        /// and implement the createConnection factory method to produce the correct Connection type
        /// on a successful ACCEPT.
        class Listener
        {
        public:

            /// @brief Destructor.
            virtual ~Listener() {}

            /// @brief Begin an asynchronous ACCEPT with this listener.
            /// One incoming Connection can be accepted for each outstanding invocation of this method.
            /// @param[in] handler EventHandler invoked when a Connection is accepted or the ACCEPT fails.
            /// @param[in] tag User-defined data passed along to the handler upon completion of this request.
            virtual void beginAccept(EventHandler handler, boost::any tag = NULL) = 0;

            /// @brief Close this Listener.
            /// Any outstanding ACCEPTS on this Listener will complete with an error.
            virtual void close() = 0;

            /// @brief Factory method for producing a Connection object accepted by this Listener.
            virtual boost::shared_ptr<Connection> createConnection() = 0;

        };
    }
}


#endif