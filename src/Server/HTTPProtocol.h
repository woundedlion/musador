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

                //////////////////////////////////////////////////////////////////////////////////////
                // Protocol State Machine

                struct StateRecvReqHeader;
		struct FSM : sc::state_machine<FSM, StateRecvReqHeader> {};

		struct EvtGotReqHeader : sc::event<EvtGotReqHeader> {};
                struct EvtGotReqData : sc::event<EvtGotReqData> {};
                struct EvtSentResHeader : sc::event<EvtSentResHeader> {};
                struct EvtSentResData : sc::event<EvtSentResData> {};


                struct StateSendResData : sc::simple_state<StateSendResData,FSM> 
                {

                };

                struct StateSendResHeader : sc::simple_state<StateSendResHeader,FSM> 
                {
                    typedef sc::transition<EvtSentResHeader, StateSendResData> reactions;
                };

                struct StateRecvReqData : sc::simple_state<StateRecvReqData,FSM> 
                {
                    typedef sc::transition<EvtGotReqData, StateSendResHeader> reactions;
                };

                struct StateRecvReqHeader : sc::simple_state<StateRecvReqHeader,FSM>
		{
	            typedef sc::transition<EvtGotReqHeader, StateRecvReqData> reactions;
		};
                
                //////////////////////////////////////////////////////////////////////////////////////

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