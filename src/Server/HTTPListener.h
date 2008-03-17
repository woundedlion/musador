#ifndef HTTP_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define HTTP_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include "SocketListener.h"

namespace Musador
{
	class HTTPListener : public SocketListener
	{
	public:

		HTTPListener(const sockaddr_in& localEP);

		boost::shared_ptr<Connection> createConnection();
	};
}

#endif