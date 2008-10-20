#ifndef ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "IO/SocketConnection.h"

namespace Musador
{
    class EchoConnection : public IO::SocketConnection
    {

    public:

        void onAcceptComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL) 
        { 
            this->beginRead(); 
        }

        void onConnectComplete(boost::shared_ptr<IO::Msg>, boost::any tag = NULL) 
        {

        }

        void onReadComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL) 
        { 
            switch (msg->getType())
            {
            case IO::MSG_READ_COMPLETE:
                {
                    boost::shared_ptr<IO::MsgReadComplete> & msgRead = boost::shared_static_cast<IO::MsgReadComplete>(msg);
                    this->beginWrite(msgRead->buf,msgRead->len);
                }
                break;
            case IO::MSG_ERROR:
                break;
            }
        }

        void onWriteComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL) 
        {
            this->beginRead();
        }
    };

}

#endif