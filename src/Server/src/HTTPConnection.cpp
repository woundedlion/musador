#include "HTTPConnection.h"
#include "HTTP.h"
#include "boost/regex.hpp"
#include "Logger/Logger.h"
#include "Connection.h"
#define LOG_SENDER L"HTTPConnection"


using namespace Musador;

HTTPConnection::HTTPConnection() :
fsm(*this)
{
	this->fsm.initiate();
}

HTTPConnection::~HTTPConnection()
{

}

void 
HTTPConnection::accepted()
{
	this->fsm.process_event(HTTP::EvtOpen());
}

void 
HTTPConnection::operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead)
{
	this->fsm.process_event(HTTP::EvtReadComplete(msgRead));
}

void 
HTTPConnection::operator<<(boost::shared_ptr<IOMsgWriteComplete> msgWrite)
{
	this->fsm.process_event(HTTP::EvtWriteComplete());
}

HTTP::FSM::FSM(Connection& conn) :
conn(conn)
{

}

// HTTP Protocol State machine logic

HTTP::StateRecvReqHeader::StateRecvReqHeader(my_context ctx) : my_base(ctx)
{
	outermost_context().conn.beginRead();
}

sc::result 
HTTP::StateRecvReqHeader::react(const HTTP::EvtReadComplete& evt)
{
	const char * start = evt.msgRead->buf.get() + evt.msgRead->off;
	const char * end = evt.msgRead->buf.get() + evt.msgRead->len;
	boost::regex expr("^(GET|POST|HEAD|OPTIONS)[[:s:]]+(?:/[^\\r\\n]*)[[:s:]]+HTTP/(1.[01])\\r\\n((?:[[:alnum:]\\-]+):[[:s:]]*(?:[^\\r\\n]*))*\\r\\n"); 
	boost::cmatch matches;
	bool valid = false;
	try
	{
		valid = boost::regex_search(start,end,matches,expr,boost::match_continuous);
	}
	catch (const std::runtime_error& e)
	{
		LOG(Error) << e.what();
	}
	if (valid)
	{
		Response& res = outermost_context().res;
		// Copy headers into Request Structure
//		for (boost::cmatch::iterator iter = matches[]	
		if (matches[1] == "GET")
		{
			res.status = 405;
			res.reason = "Method Not Supported";
			return transit<StateReqError>();
		}
		else if(matches[1] == "POST")
		{
			res.status = 405;
			res.reason = "Method Not Supported";
			return transit<StateReqError>();
		}
		else if (matches[1] == "HEAD")
		{
			res.status = 405;
			res.reason = "Method Not Supported";
			return transit<StateReqError>();
		}
		else if (matches[1] == "OPTIONS")
		{
			res.status = 405;
			res.reason = "Method Not Supported";
			return transit<StateReqError>();
		}
		else
		{
			res.status = 405;
			res.reason = "Method Not Supported";
			return transit<StateReqError>();
		}
	}
	else // Haven't received the full header yet
	{
		if (evt.msgRead->off == evt.msgRead->len)
		{
			LOG(Warning) << "Discarding " << evt.msgRead->len << " bytes receivied without a valid header.";
			outermost_context().conn.beginRead(); // Discard overflowed message, keep reading into a new message buffer
		}
		else
		{
			outermost_context().conn.beginRead(evt.msgRead); // keep reading into the same message buffer
		}
		return discard_event();
	}
}

HTTP::StateReqError::StateReqError(my_context ctx) : my_base(ctx)
{
	HTTP::Response& res = outermost_context().res;
	res.data << "<h1>" << res.status << " " << res.reason << "</h1>";
	res.headers["Content-Type"] = "text/html";
	res.headers["Content-Length"] = boost::lexical_cast<std::string>(res.data.tellp());
	post_event(EvtReqDone());
}

HTTP::StateRecvReqBodyChunk::StateRecvReqBodyChunk(my_context ctx) : my_base(ctx)
{

}

sc::result 
HTTP::StateRecvReqBodyChunk::react(const EvtReadComplete& evt)
{
	return discard_event();
}

HTTP::StateRecvReqBody::StateRecvReqBody(my_context ctx) : my_base(ctx)
{

}

sc::result 
HTTP::StateRecvReqBody::react(const EvtReadComplete& evt)
{
	return discard_event();
}

HTTP::StateSendResHeader::StateSendResHeader(my_context ctx) : my_base(ctx)
{

}

sc::result 
HTTP::StateSendResHeader::react(const EvtWriteComplete& evt)
{
	return discard_event();
}

HTTP::StateSendResBodyChunk::StateSendResBodyChunk(my_context ctx) : my_base(ctx)
{

}

sc::result 
HTTP::StateSendResBodyChunk::react(const EvtWriteComplete& evt)
{
	return discard_event();
}

HTTP::StateSendResBody::StateSendResBody(my_context ctx) : my_base(ctx)
{

}

sc::result 
HTTP::StateSendResBody::react(const EvtWriteComplete& evt)
{
	return discard_event();
}







