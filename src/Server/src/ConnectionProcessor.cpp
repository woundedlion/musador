#include <boost/bind.hpp>
#include "ConnectionProcessor.h"

using namespace Musador;

ConnectionProcessor::ConnectionProcessor()
{
	this->processorThread = new boost::thread(boost::bind(&ConnectionProcessor::run,this));
}

ConnectionProcessor::~ConnectionProcessor()
{
	this->shutdown();
	this->processorThread->join();
	delete this->processorThread;
}

void ConnectionProcessor::post(boost::shared_ptr<IOMsg> msg,boost::any tag)
{
	MessageSink<IOMsg>::post(msg);
}	

void ConnectionProcessor::run()
{
	while (true)
	{
		boost::shared_ptr<IOMsg> msg = this->popMsg();
		switch (msg->getType())
		{
		case IO_SHUTDOWN:
			return;			
		case IO_READ_COMPLETE:
			{
				boost::shared_ptr<IOMsgReadComplete> msgRead(boost::shared_static_cast<IOMsgReadComplete>(msg));
				msgRead->conn->post(msgRead);
			}
			break;
		case IO_WRITE_COMPLETE:
			{
				boost::shared_ptr<IOMsgWriteComplete> msgWrite(boost::shared_static_cast<IOMsgWriteComplete>(msg));
				msgWrite->conn->post(msgWrite);
			}
			break;
		case IO_ERROR:
			{

			}
			break;
		}

	}
}

void ConnectionProcessor::shutdown()
{
	MessageSink<IOMsg>::post(boost::shared_ptr<IOMsg>(new IOMsgShutdown()));
}

