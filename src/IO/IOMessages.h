#ifndef IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D
#define IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <cassert>
#include "Network/Network.h"
#include "BufferChain.h"

namespace Musador
{
    namespace IO
    {
        class Connection;
        class Listener;

        /// @enum MsgType List of IO message types
        enum MsgType
        {
            MSG_WRITE_COMPLETE,
            MSG_READ_COMPLETE,
            MSG_SOCKET_ACCEPT_COMPLETE,
            MSG_PIPE_ACCEPT_COMPLETE,
            MSG_SOCKET_CONNECT_COMPLETE,
            MSG_PIPE_CONNECT_COMPLETE,
            MSG_INVOKE
        };

        /// @class Msg
        /// @brief Base class for all IO messages.
        /// Messages derived from this class are used to deliver completion events.
        class Msg
        {
        public:

            /// @brief System-specific error code type
            typedef int ErrorCode;

            /// @brief constructor.
            /// @param[in] type A member of the MsgType enumeration indicating the type of this message.
            Msg(MsgType type) : type(type), error(0) {}

            /// @brief Get the type of this connection.
            /// @returns A member of the MsgType enumeration indicating the type of this message.
            inline MsgType getType() { return type; }

            /// @brief Check if this message indicates an error
            /// @returns True if this Msg indicates an error
            bool isError() { return error != 0; }

            /// @brief Get the system-specific error code
            /// @returns An error code
            ErrorCode getError() const { return this->error; }

            /// @brief Set the system-specific error code
            /// @param[in] code An error code
            void setError(ErrorCode code) { this->error = code; }

        private: 

            MsgType type;
            ErrorCode error;
        };

        /// @class MsgWriteComplete
        /// @brief Message indicating completion of a WRITE request.
        class MsgWriteComplete : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgWriteComplete() : Msg(MSG_WRITE_COMPLETE),
                buf(MsgWriteComplete::MAX)
            {
            }

            /// @brief Constructor.
            /// @param[in] buf Buffer containing existing data to write
            inline MsgWriteComplete(const Buffer<char>& buf) : Msg(MSG_WRITE_COMPLETE),
                buf(buf)
            {
            }

            /// @brief A shared pointer to the Connection object to which this message refers.
            boost::shared_ptr<Connection> conn;

            /// @brief Buffer containing the data that was requested to be written.
            Buffer<char> buf;
            
            /// @brief The maximum capacity or size of the buffer pointed to by buf.
            static const int MAX = 4096;
        };

        /// @class MsgReadComplete
        /// @brief Message indicating completion of a READ request.
        class MsgReadComplete : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgReadComplete() : Msg(MSG_READ_COMPLETE),
                buf(MsgReadComplete::MAX)
            {
            }

            /// @brief Constructor.
            /// @param[in] buf Buffer containing existing data to write
            inline MsgReadComplete(const Buffer<char>& buf) : Msg(MSG_READ_COMPLETE),
                buf(buf)
            {
            }

            /// @brief A shared pointer to the Connection object to which this message refers.
            boost::shared_ptr<Connection> conn;

            /// @brief Buffer containing the data which was read.
            Buffer<char> buf;

            /// @brief The Maximum capacity or size of buf.
            static const int MAX = 4096;
        };

        /// @class MsgSocketAcceptComplete
        /// @brief Message indicating completion of a Socket ACCEPT request.
        class MsgSocketAcceptComplete : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgSocketAcceptComplete() : Msg(MSG_SOCKET_ACCEPT_COMPLETE),
                buf(2 * (sizeof(sockaddr_in) + 16))
            {
            }

            /// @brief A shared pointer to the Connection object to which this message refers.
            boost::shared_ptr<Connection> conn;

            /// @brief Shared pointer to the Listener object which accepted this connection.
            boost::shared_ptr<Listener> listener;

            /// @brief Buffer containing any data which was read along with the ACCEPT.
            Buffer<char> buf;
        };

        /// @class MsgPipeAcceptComplete
        /// @brief Message indicating completion of a Pipe ACCEPT request.
       class MsgPipeAcceptComplete : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgPipeAcceptComplete() : Msg(MSG_PIPE_ACCEPT_COMPLETE)
            {
            }

            /// @brief A shared pointer to the Connection object to which this message refers.
            boost::shared_ptr<Connection> conn;

            /// @brief Shared pointer to the Listener object which accepted this connection.
            boost::shared_ptr<Listener> listener;
        };

       /// @class MsgSocketConnectComplete
       /// @brief Message indicating completion of a Socket CONNECT request.
        class MsgSocketConnectComplete : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgSocketConnectComplete() : Msg(MSG_SOCKET_CONNECT_COMPLETE)
            {
            }

            /// @brief A shared pointer to the Connection object to which this message refers.
            boost::shared_ptr<Connection> conn;
        };

        /// @class MsgPipeConnectComplete
        /// @brief Message indicating completion of a Pipe CONNECT request.
        class MsgPipeConnectComplete : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgPipeConnectComplete() : Msg(MSG_PIPE_CONNECT_COMPLETE)
            {
            }

            /// @brief A shared pointer to the Connection object to which this message refers.
            boost::shared_ptr<Connection> conn;
        };

        class MsgInvoke : public Msg
        {
        public:

            /// @brief Constructor.
            inline MsgInvoke() : Msg(MSG_INVOKE)
            {
            }
        };

        /// @brief Type of the callback invoked for completion events.
        /// Currently, this is a function pointer with the following signature: 
        /// void callback(boost::shared_ptr<Msg>, boost::any tag);
        /// For examples of EventHandler implementations, 
        /// see Connection::onReadComplete, Connection::onWriteComplete, etc.
        typedef boost::function2<void, boost::shared_ptr<Msg>, boost::any> EventHandler;

    }
}

#include "Connection.h"

#endif