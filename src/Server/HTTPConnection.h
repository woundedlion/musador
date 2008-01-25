#ifndef HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474
#define HTTP_PROTOCOL_B85D188B_14AE_402b_8977_BEB491AAB474

#include <queue>

#include "HTTP.h"
#include "Connection.h"

#include "boost/weak_ptr.hpp"
#include "boost/statechart/state_machine.hpp"
#include "boost/statechart/event.hpp"
#include "boost/statechart/simple_state.hpp"
#include "boost/statechart/state.hpp"
#include "boost/statechart/transition.hpp"
#include "boost/statechart/custom_reaction.hpp"
#include <boost/mpl/list.hpp>

namespace mpl = boost::mpl;
namespace sc = boost::statechart;

namespace Musador
{

	namespace HTTP
	{
		//////////////////////////////////////////////////////////////////////////////////////
		// Protocol State Machine

		// States
		struct StateRecvReq;
		struct StateRecvReqHeader;
		struct StateRecvReqBody;
		struct StateRecvReqBodyChunk;
		struct StateReqError;
		struct StateSendRes;
		struct StateSendResHeader;
		struct StateSendResBody;
		struct StateSendResBodyChunk;

		// Events
		struct EvtError : sc::event<EvtError> {};
		struct EvtRecvdReqHeader : sc::event<EvtRecvdReqHeader> {};
		struct EvtRecvdReqChunk : sc::event<EvtRecvdReqChunk> {};
		struct EvtRecvdReq : sc::event<EvtRecvdReq> {};
		struct EvtSentResHeader : sc::event<EvtSentResHeader> {};
		struct EvtSentResChunk : sc::event<EvtSentResChunk> {};
		struct EvtSentRes : sc::event<EvtSentRes> {};

		// State definitions, transitions
		struct FSM : sc::state_machine<FSM,StateRecvReq>
		{
			FSM(Connection& conn);

			Connection& conn;
			std::auto_ptr<HTTP::Request> req;
			std::auto_ptr<HTTP::Response> res;
			std::queue<boost::shared_ptr<IOMsgReadComplete> > msgsIn;
		};

		struct StateRecvReq : sc::simple_state<StateRecvReq,FSM,StateRecvReqHeader>
		{
			typedef sc::transition<EvtRecvdReq,StateSendRes> reactions;
			
		};

		struct StateRecvReqHeader : sc::state<StateRecvReqHeader,StateRecvReq>
		{
			typedef mpl::list<
				sc::transition<EvtError,StateReqError>,
				sc::custom_reaction<EvtRecvdReqHeader>
			> reactions;

			StateRecvReqHeader(my_context ctx);
			sc::result react(const EvtRecvdReqHeader& evt);

		};

		struct StateReqError : sc::simple_state<StateReqError,StateRecvReq>
		{

		};

		struct StateRecvReqBodyChunk : sc::simple_state<StateRecvReqBodyChunk,StateRecvReq>
		{
			typedef sc::transition<EvtRecvdReqChunk,StateRecvReqBodyChunk> reactions;			
		};

		struct StateRecvReqBody : sc::simple_state<StateRecvReqBody,StateRecvReq>
		{

		};

		struct StateSendRes : sc::simple_state<StateSendRes,FSM,StateSendResHeader>
		{
			typedef sc::custom_reaction<EvtSentRes> reactions;	

			sc::result react(const EvtSentRes& evt);
		};

		struct StateSendResHeader : sc::simple_state<StateSendResHeader,StateSendRes>
		{
			typedef sc::custom_reaction<EvtSentResHeader> reactions;	

			sc::result react(const EvtSentResHeader& evt);
		};

		struct StateSendResBodyChunk : sc::simple_state<StateSendResBodyChunk,StateSendRes>
		{
			typedef sc::transition<EvtSentResChunk,StateSendResBodyChunk> reactions;			
		};

		struct StateSendResBody : sc::simple_state<StateSendResBody,StateSendRes>
		{

		};
	}

	class HTTPConnection : public Connection
	{
	public:

		HTTPConnection();

		~HTTPConnection();

		void accepted();

		void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead);

	private:

		HTTP::FSM fsm;
	};

	class HTTPConnectionFactory : public ConcreteFactory<Connection,HTTPConnection> { };


}

#endif