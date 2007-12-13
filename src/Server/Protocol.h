#ifndef PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474
#define PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474

#include "boost/shared_ptr.hpp"
#include "IOMessages.h"

namespace Musador
{
	class Protocol
	{
	public:
		virtual void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead) = 0;
	};

}

#endif