#ifndef ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "Logger/Logger.h"
#include "IO/SocketConnection.h"

namespace Musador
{
    /// @class EchoConnection
    /// @brief Simple Echo Server Connection
    /// A connection type which simply sends back whatever data it receives.
    /// Useful mostly for testing.
    class EchoConnection : public IO::SocketConnection
    {

    public:

        /// @brief Completion routine invoked by the IO engine when a connection is first accepted.
        /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous ACCEPT.
        /// @param[in] tag User-defined data which was originally passed into Listener::beginAccept.
        void onAcceptComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL) 
        { 
            this->beginRead(); 
        }

        /// @brief Completion routine invoked by the IO engine when a connection is first connected.
        /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous CONNECT.
        /// Currently, this is either a MsgConnectComplete or MsgError.
        /// @param[in] tag User-defined data which was originally passed into beginConnect.
        void onConnectComplete(boost::shared_ptr<IO::Msg>, boost::any tag = NULL) 
        {

        }

        /// @brief Completion routine invoked by the IO engine whenever an asynchronous READ 
        /// on this connection completes.
        /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous READ.
        /// Currently, this is either a MsgReadComplete or MsgError.
        /// @param[in] tag User-defined data which was originally passed into beginRead.
        void onReadComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL) 
        { 
            assert(msg->getType() == IO::MSG_READ_COMPLETE);
            boost::shared_ptr<IO::MsgReadComplete> & msgRead = boost::shared_static_cast<IO::MsgReadComplete>(msg);
            if (msgRead->isError()) 
            {
                log(Error, L"EchoConnection") << "Error reading from Echo connection: " << msgRead->getError();
            }
            else 
            {
                this->beginWrite(msgRead->buf);
            }
        }

        /// @brief Completion routine invoked by the IO engine whenever an asynchronous WRITE 
        /// on this connection completes.
        /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous WRITE.
        /// Currently, this is either a MsgWriteComplete or MsgError.
        /// @param[in] tag User-defined data which was originally passed into beginWrite.
        void onWriteComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL) 
        {
            this->beginRead();
        }
    };

}

#endif