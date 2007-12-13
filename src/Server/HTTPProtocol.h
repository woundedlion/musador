#ifndef HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474
#define HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474

#include "Protocol.h"
#include "HTTP.h"

namespace Musador
{

	class HTTPProtocol : public Protocol
	{
	public:

		HTTPProtocol();
		
		~HTTPProtocol();

		void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead);

	private:

		typedef std::vector<boost::shared_ptr<HTTP::StateStore> > SessionCollection;
		typedef std::vector<boost::shared_ptr<HTTP::User> > UserCollection;

		UserCollection users;
		Mutex usersMutex;
		SessionCollection sessions;
		Mutex sessionsMutex;

	};
}
#endif