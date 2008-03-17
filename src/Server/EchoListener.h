#ifndef ECHO_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define ECHO_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include "SocketListener.h"

namespace Musador
{
	class EchoListener : public SocketListener
	{
	public:

		EchoListener(const sockaddr_in& localEP);

		boost::shared_ptr<Connection> createConnection();
	};
}

#endif