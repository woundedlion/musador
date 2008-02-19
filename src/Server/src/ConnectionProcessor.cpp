#include <boost/bind.hpp>
#include "ConnectionProcessor.h"
#include "Server.h"
#include <assert.h>

using namespace Musador;

ConnectionProcessor::ConnectionProcessor() :
processorThread(NULL)
{
}

ConnectionProcessor::~ConnectionProcessor()
{
	this->shutdown();
}

void ConnectionProcessor::post(boost::shared_ptr<IOMsg> msg,boost::any tag)
{
	MessageSink<IOMsg>::post(msg);
}	

void ConnectionProcessor::_run()
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
				boost::shared_ptr<IOMsgError> msgErr(boost::shared_static_cast<IOMsgError>(msg));
				msgErr->conn->getCtx()->server->onError(msgErr);
			}
			break;
		}

	}
}

void ConnectionProcessor::start()
{
	assert(NULL == this->processorThread);
	this->processorThread = new boost::thread(boost::bind(&ConnectionProcessor::_run,this));
}

void ConnectionProcessor::shutdown()
{
	if (NULL != this->processorThread)
	{
		MessageSink<IOMsg>::post(boost::shared_ptr<IOMsg>(new IOMsgShutdown()));
		this->processorThread->join();
		delete this->processorThread;
		this->processorThread = NULL;
	}
}

