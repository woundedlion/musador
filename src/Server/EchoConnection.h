#ifndef ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define ECHO_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "SocketConnection.h"

namespace Musador
{
	class EchoConnection : public SocketConnection
	{

	public:

		void accepted() { this->beginRead(); }
		
		void post(boost::shared_ptr<IOMsgReadComplete> msgRead) 
		{ 
			this->beginWrite(msgRead->buf,msgRead->len);
		}

		void post(boost::shared_ptr<IOMsgWriteComplete> msgWrite) 
		{
			this->beginRead();
		}
	};

}

#endif