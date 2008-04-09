#ifndef ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "SocketConnection.h"

namespace Musador
{
	class EchoConnection : public SocketConnection
	{

	public:

		void accepted(boost::any tag = NULL) { this->beginRead(); }
		
		void onConnectComplete(boost::shared_ptr<IOMsg>, boost::any tag = NULL) {}

		void onReadComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL) 
		{ 
			switch (msg->getType())
			{
			case IO_READ_COMPLETE:
				{
					boost::shared_ptr<IOMsgReadComplete> & msgRead = boost::shared_static_cast<IOMsgReadComplete>(msg);
					this->beginWrite(msgRead->buf,msgRead->len);
				}
				break;
			case IO_ERROR:
				break;
			}
		}

		void onWriteComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL) 
		{
			this->beginRead();
		}
	};

}

#endif