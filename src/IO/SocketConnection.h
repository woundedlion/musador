#ifndef SOCKET_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define SOCKET_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"

namespace Musador
{
    namespace IO
    {
        /// @class SocketConnection
        /// @brief Abstract base class for Socket connections.
        /// Provides an asynchronous interface for performing I/O on a Socket.
        /// Specific protocol implementations, e.g. HTTPConnection, EchoConnection
        /// are derived from this class and implement the on* methods to provide protocol logic.
        class SocketConnection : public Connection, public boost::enable_shared_from_this<SocketConnection>
        {
        public:

            /// @brief Constructor.
            SocketConnection();

            /// @brief Destructor.
            ~SocketConnection();

            /// @brief Start an asynchronous CONNECT.
            /// Upon completion, the IO engine will invoke the onConnectComplete method.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onConnectComplete.
            virtual void beginConnect(boost::any tag = NULL) {}

            /// @brief Start an asynchronous READ.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onReadComplete.
            void beginRead(boost::any tag = NULL);

            /// @brief Start an asynchronous READ using a previously allocated read completion message.
            /// Upon completion, the IO engine will invoke the onReadComplete method.
            /// @param[in] msgRead Msg object used to store any data read from the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onReadComplete.
            void beginRead(boost::shared_ptr<MsgReadComplete> msgRead, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using a previously allocated write completion message.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] msgWrite Msg object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(boost::shared_ptr<MsgWriteComplete> msgWrite, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using a previously allocated array.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] data Buffer object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(const Buffer<char>& data, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using data from an input stream.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] dataStream std::istream object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(std::istream& dataStream, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using data from a std::string.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] str std::string object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(const std::string& str, boost::any tag = NULL);

            /// @brief Close the underlying Socket.
            void close();

            /// @brief Get a string representation of this SocketConnection.
            /// @returns A string of the for X.X.X.X:Y representing the IP address and port 
            /// to which the underlying socket is bound.
            std::string toString();

            /// @brief Get this SocketConnection's underlying Socket.
            /// @returns a system-specific handle to this SocketConnection's underlying Socket.
            SOCKET getSocket();

            /// @brief Set the underlying Socket instance used by this SocketConnection.
            /// @param[in] sock A system-specific handle to a Socket instance to be used for this SocketConnection.
            void setSocket(SOCKET sock);

            /// @brief Get the local IP endpoint of this SocketConnection.
            /// @returns a sockaddr_in structure containing this SocketConnection's local endpoint information.
            sockaddr_in getLocalEP();

            /// @brief Set the local IP endpoint of this SocketConnection.
            /// @param[in] localEP A sockaddr_in structure containing this SocketConnection's local endpoint information.
            void setLocalEP(sockaddr_in localEP);

            /// @brief Get the remote IP endpoint of this SocketConnection.
            /// @returns a sockaddr_in structure containing this SocketConnection's remote endpoint information.
            sockaddr_in getRemoteEP();

            /// @brief Set the remote IP endpoint of this SocketConnection.
            /// @param[in] localEP A sockaddr_in structure containing this SocketConnection's remote endpoint information.
            void setRemoteEP(sockaddr_in localEP);

        private:

            sockaddr_in localEP;
            sockaddr_in remoteEP;
            SOCKET sock;

        };
    }
}

#endif