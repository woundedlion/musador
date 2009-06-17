#ifndef CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <sstream>
#include "boost/function.hpp"
#include <boost/shared_ptr.hpp>
#include "boost/any.hpp"
#include "Network/Network.h"
#include "Utilities/Factory.h"
#include "IOMessages.h"

namespace Musador
{
    class Server;

    namespace IO
    {

        /// @class ConnectionCtx
        /// @brief Base class for contextual information carried along with each connection instance.
        class ConnectionCtx
        {
        };

        /// @class Connection
        /// @brief Abstract base class for all types of connections.
        /// Provides an asynchronous interface for performing I/O on various types of connections.
        /// In general, concrete connection types, e.g. SocketConnection and PipeConnection derive
        /// from this class and implement the begin* methods.
        /// Specific protocol implementations, e.g. HTTPConnection and EchoConnection 
        /// are then derived from the concrete connection types
        /// and implement the on* methods to provide protocol logic.
        class Connection
        {
        public:

            /// @brief Constructor
            virtual ~Connection() {};

            /// @brief Start an asynchronous CONNECT.
            /// Upon completion, the IO engine will invoke the onConnectComplete method.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onConnectComplete.
            virtual void beginConnect(boost::any tag = NULL) = 0;

            /// @brief Start an asynchronous READ.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onReadComplete.
            virtual void beginRead(boost::any tag = NULL) = 0;

            /// @brief Start an asynchronous READ using a previously allocated read completion message.
            /// Upon completion, the IO engine will invoke the onReadComplete method.
            /// @param[in] msgRead Msg object used to store any data read from the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onReadComplete.
            virtual void beginRead(boost::shared_ptr<MsgReadComplete> msgRead, boost::any tag = NULL) = 0;

            /// @brief Start an asynchronous WRITE using a previously allocated write completion message.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] msgWrite Msg object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            virtual void beginWrite(boost::shared_ptr<MsgWriteComplete> msgWrite, boost::any tag = NULL) = 0;

            /// @brief Start an asynchronous WRITE using a previously allocated array.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] data Buffer object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            virtual void beginWrite(const Buffer<char>& data, boost::any tag = NULL) = 0;

            /// @brief Start an asynchronous WRITE using data from an input stream.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] dataStream std::istream object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            virtual void beginWrite(std::istream& dataStream, boost::any tag = NULL) = 0;

            /// @brief Start an asynchronous WRITE using data from a std::string.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] str std::string object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            virtual void beginWrite(const std::string& str, boost::any tag = NULL) = 0;

            /// @brief Completion routine invoked by the IO engine when a connection is first accepted.
            /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous ACCEPT.
            /// @param[in] tag User-defined data which was originally passed into Listener::beginAccept.
            virtual void onAcceptComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;

            /// @brief Completion routine invoked by the IO engine when a connection is first connected.
            /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous CONNECT.
            /// @param[in] tag User-defined data which was originally passed into beginConnect.
            virtual void onConnectComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;		

            /// @brief Completion routine invoked by the IO engine whenever an asynchronous READ 
            /// on this connection completes.
            /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous READ.
            /// @param[in] tag User-defined data which was originally passed into beginRead.
            virtual void onReadComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;

            /// @brief Completion routine invoked by the IO engine whenever an asynchronous WRITE 
            /// on this connection completes.
            /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous WRITE.
            /// @param[in] tag User-defined data which was originally passed into beginWrite.
            virtual void onWriteComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;

            /// @brief Close the connection.
            virtual void close() = 0;

            /// @brief Get a string representation of this connection.
            /// @returns A string representing this connection.
            virtual std::string toString() = 0;

        };
    }
}

#endif
