#ifndef CONNECTION_PROCESSOR_H_09B3F66A_2036_4881_8857_EA5D7429AB40
#define CONNECTION_PROCESSOR_H_09B3F66A_2036_4881_8857_EA5D7429AB40

#include <boost/thread.hpp>
#include <boost/any.hpp>
#include "Utilities/MessageSink.h"
#include "Utilities/Singleton.h"
#include "IOMessages.h"

namespace Musador
{

	class ConnectionProcessor : public MessageSink<IOMsg>
	{
	public:

		ConnectionProcessor();
		
		~ConnectionProcessor();

		void post(boost::shared_ptr<IOMsg> msg,boost::any tag);

		void _run();

		void start();

		void shutdown();

	private:
		
		boost::thread * processorThread;
	};
}

#endif