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
		struct StateClosed;
		struct StateRecvReq;
		struct StateRecvReqHeader;
		struct StateRecvReqBody;
		struct StateRecvReqBodyChunk;
		struct StateReqError;
		struct StateSendRes;
		struct StateSendResHeader;
		struct StateSendResBody;
		struct StateSendResBodyChunk;
		struct StateReqProcess;

		// Events
		struct EvtOpen : sc::event<EvtOpen> {};
		struct EvtReadComplete : sc::event<EvtReadComplete> 
		{
			EvtReadComplete(boost::shared_ptr<IOMsgReadComplete> msgRead) : msgRead(msgRead) {}
			boost::shared_ptr<IOMsgReadComplete> msgRead;
		};
		struct EvtReqDone : sc::event<EvtReqDone> {};
		struct EvtWriteComplete : sc::event<EvtWriteComplete> {};
		struct EvtClose : sc::event<EvtClose> {};
		struct EvtKeepAlive : sc::event<EvtKeepAlive> {};

		// State definitions, transitions
		struct FSM : sc::state_machine<FSM,StateClosed>
		{
			FSM(Connection& conn);

			Connection& conn;
			HTTP::Env env;
		};

		struct StateClosed : sc::simple_state<StateClosed,FSM>
		{
			typedef sc::transition<EvtOpen,StateRecvReq> reactions;
		};

		struct StateRecvReq : sc::simple_state<StateRecvReq,FSM,StateRecvReqHeader>
		{
			typedef sc::transition<EvtReqDone,StateSendRes> reactions;
			
		};

		struct StateRecvReqHeader : sc::state<StateRecvReqHeader,StateRecvReq>
		{
			typedef mpl::list<
				sc::custom_reaction<EvtReadComplete>
			> reactions;

			StateRecvReqHeader(my_context ctx);
			sc::result react(const EvtReadComplete& evt);

		};

		struct StateReqError : sc::state<StateReqError,StateRecvReq>
		{
			StateReqError(my_context ctx);
		};

		struct StateRecvReqBodyChunk : sc::state<StateRecvReqBodyChunk,StateRecvReq>
		{
			typedef mpl::list<
				sc::custom_reaction<EvtReadComplete>
			> reactions;

			StateRecvReqBodyChunk(my_context ctx);
			sc::result react(const EvtReadComplete& evt);
		};

		struct StateRecvReqBody : sc::state<StateRecvReqBody,StateRecvReq>
		{
			typedef mpl::list<
				sc::custom_reaction<EvtReadComplete>
			> reactions;

			StateRecvReqBody(my_context ctx);
			sc::result react(const EvtReadComplete& evt);
		};

		struct StateReqProcess : sc::state<StateReqProcess,StateRecvReq>
		{
			StateReqProcess(my_context ctx);
		};

		struct StateSendRes : sc::simple_state<StateSendRes,FSM,StateSendResHeader>
		{
			typedef mpl::list<
				typedef sc::transition<EvtClose,StateClosed>,	
				typedef sc::transition<EvtKeepAlive,StateRecvReq>
			> reactions;
		};

		struct StateSendResHeader : sc::state<StateSendResHeader,StateSendRes>
		{
			typedef mpl::list<
				sc::custom_reaction<EvtWriteComplete>
			> reactions;

			StateSendResHeader(my_context ctx);
			sc::result react(const EvtWriteComplete& evt);
		};

		struct StateSendResBodyChunk : sc::state<StateSendResBodyChunk,StateSendRes>
		{
			typedef mpl::list<
				sc::custom_reaction<EvtWriteComplete>
			> reactions;

			StateSendResBodyChunk(my_context ctx);
			sc::result react(const EvtWriteComplete& evt);		
		};

		struct StateSendResBody : sc::state<StateSendResBody,StateSendRes>
		{
			typedef mpl::list<
				sc::custom_reaction<EvtWriteComplete>
			> reactions;

			StateSendResBody(my_context ctx);
			sc::result react(const EvtWriteComplete& evt);
		};

	}


	class HTTPConnection : public Connection
	{
	public:

		HTTPConnection();

		~HTTPConnection();

		void accepted();

		void post(boost::shared_ptr<IOMsgReadComplete> msgRead);

		void post(boost::shared_ptr<IOMsgWriteComplete> msgWrite);

	private:

		HTTP::FSM fsm;

		static ConnectionProcessor processor;
	};

	class HTTPConnectionFactory : public ConcreteFactory<Connection,HTTPConnection> { };


}

#endif