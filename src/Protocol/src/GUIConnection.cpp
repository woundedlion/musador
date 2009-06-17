#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/bind.hpp>
#include "Utilities/TimerQueue.h"
#include "GUIConnection.h"
#include "GUIMessages.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"HTTPConnection"

using namespace Musador;

GUIConnection::GUIConnection() :
IO::PipeConnection(GUI_PIPE_NAME),
connRetries(0)
{
}

void
GUIConnection::onReadComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /* = NULL */)
{
    assert(msg->getType() == IO::MSG_READ_COMPLETE);
    boost::shared_ptr<IO::MsgReadComplete> & msgRead = boost::shared_static_cast<IO::MsgReadComplete>(msg);
    if (msgRead->err.success()) 
    {
        if (NULL != this->handler)
        {
            boost::shared_ptr<IO::MsgReadComplete> & msgRead = boost::shared_static_cast<IO::MsgReadComplete>(msg);
            // parse the messsage(s)
            std::stringbuf msgData;
            msgData.pubsetbuf(0,0);
            msgData.str(std::string(msgRead->buf.begin(), msgRead->buf.end()));
            boost::archive::binary_iarchive ar(msgData);
            boost::shared_ptr<GUIMsg> msgGUI;
            ar & msgGUI; // deserialize
            this->handler(msgGUI);
        }				

        // keep reading more messages
        this->beginRead();
    } 
    else
    {
        if (NULL != this->handler)
        {
            boost::shared_ptr<GUIMsg> msgGUI(new GUIMsgDisabledNotify());
            this->handler(msgGUI);
        }
        this->beginConnect();
    }
}

void
GUIConnection::onWriteComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /* = NULL */)
{
} 

void 
GUIConnection::onAcceptComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /*= NULL*/)
{
}

void 
GUIConnection::onConnectComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /* = NULL */)
{
    assert (msg->getType() == IO::MSG_PIPE_CONNECT_COMPLETE);
    boost::shared_ptr<IO::MsgPipeConnectComplete>& msgConnect = boost::shared_static_cast<IO::MsgPipeConnectComplete>(msg);
    if (msgConnect->err.success()) 
    {
        this->beginRead();
    }
    else
    {
        LOG(Debug) << "GUI could not connect to service: " << msgConnect->err.get();
        Util::TimerQueue::instance()->createTimer(500, boost::bind(&PipeConnection::beginConnect,this,tag));
    }
}

void
GUIConnection::setHandler(const GUIHandler& handler)
{
    this->handler = handler;
}