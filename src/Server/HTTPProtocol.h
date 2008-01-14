#ifndef HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474
#define HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474

#include "Protocol.h"
#include "HTTP.h"

#include "boost/statechart/state_machine.hpp"
#include "boost/statechart/event.hpp"
#include "boost/statechart/simple_state.hpp"
#include <boost/statechart/transition.hpp>

namespace sc = boost::statechart;

namespace Musador
{
	class HTTPProtocol : public Protocol
	{
	public:

		struct StateRecvReqHeader;
		struct FSM : sc::state_machine<FSM, StateRecvReqHeader> {};

		struct EvtGotHeader : sc::event<EvtGotHeader> {};

		struct StateRecvReqData : sc::simple_state<StateRecvReqData,FSM> {};

		struct StateRecvReqHeader : sc::simple_state<StateRecvReqHeader,FSM>
		{
			typedef sc::transition< EvtGotHeader, StateRecvReqData > reactions;
		};


		HTTPProtocol();
		
		~HTTPProtocol();

		void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead);

	private:
    
			std::auto_ptr<HTTP::Request> req;
			std::auto_ptr<HTTP::Response> res;

			void error(Connection& conn, int errCode, const char * errMsg);

			void readReqHeader(boost::shared_ptr<IOMsgReadComplete> msgRead);
			void readReqData(boost::shared_ptr<IOMsgReadComplete> msgRead);

        };

        class HTTPProtocolFactory : public ConcreteFactory<Protocol,HTTPProtocol> { };
}
#endif