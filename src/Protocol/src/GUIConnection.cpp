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
PipeConnection(GUI_PIPE_NAME),
connRetries(0)
{
}

void
GUIConnection::onReadComplete(boost::shared_ptr<IOMsg> msg, boost::any tag /* = NULL */)
{
	switch (msg->getType())
	{
	case IO_READ_COMPLETE:
		if (NULL != this->handler)
		{
			boost::shared_ptr<IOMsgReadComplete> & msgRead = boost::shared_static_cast<IOMsgReadComplete>(msg);
			// parse the messsage(s)
			std::stringbuf msgData;
			msgData.pubsetbuf(0,0);
			msgData.str(std::string(msgRead->buf.get(),msgRead->len));
			boost::archive::binary_iarchive ar(msgData);
			boost::shared_ptr<GUIMsg> msgGUI;
			ar & msgGUI; // deserialize
			this->handler(msgGUI);
		}				

		// keep reading more messages
		this->beginRead();
		break;
	case IO_ERROR:
		this->beginConnect();
		break;
	}
}

void
GUIConnection::onWriteComplete(boost::shared_ptr<IOMsg> msg, boost::any tag /* = NULL */)
{

} 

void 
GUIConnection::onAcceptComplete(boost::shared_ptr<IOMsg> msg, boost::any tag /*= NULL*/)
{
}

void 
GUIConnection::onConnectComplete(boost::shared_ptr<IOMsg> msg, boost::any tag /* = NULL */)
{
	switch (msg->getType())
	{
	case IO_PIPE_CONNECT_COMPLETE:
		this->beginRead();
		break;
	case IO_ERROR:
                TimerQueue::instance()->createTimer(500, boost::bind(&PipeConnection::beginConnect,this,tag));
		break;
	}
}

void
GUIConnection::setHandler(const GUIHandler& handler)
{
	this->handler = handler;
}