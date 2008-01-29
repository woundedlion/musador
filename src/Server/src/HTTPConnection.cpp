#include "HTTPConnection.h"
#include "HTTP.h"
#include "boost/regex.hpp"
#include "Logger/Logger.h"
#include "Connection.h"
#include "boost/algorithm/string.hpp"

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

/////////////////////////////////////////////////////////////////////////////////////
// HTTP Protocol State machine logic
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// Receive States
/////////////////////////////////////////////////////////////////////////////////////

HTTP::StateRecvReqHeader::StateRecvReqHeader(my_context ctx) : my_base(ctx)
{
	outermost_context().env.req.clear();
	outermost_context().env.res.clear();
	outermost_context().conn.beginRead();
}

sc::result 
HTTP::StateRecvReqHeader::react(const HTTP::EvtReadComplete& evt)
{
	const char * start = evt.msgRead->buf.get() + evt.msgRead->off;
	const char * end = evt.msgRead->buf.get() + evt.msgRead->len;
	bool valid = false;
	boost::regex expr("([[:alpha:]]+)[[:s:]]+([^[:s:]?]+)(?:\\?(\\S*))?[[:s:]]+(HTTP/1.[01])\\r\\n(?:([[:alnum:]\\-]+):[[:s:]]*([^\\r\\n]*)\\r\\n)*\\r\\n"); 
	boost::cmatch matches;
	try
	{
		valid = boost::regex_search(start,end,matches,expr,boost::match_extra);
	}
	catch (const std::runtime_error& e)
	{
		LOG(Error) << e.what();
	}
	if (valid)
	{
		Request& req = outermost_context().env.req;
		Response& res = outermost_context().env.res;
		
		// Fill the request object
		req.method = matches[1];
		req.requestURI = matches[2];
		res.protocol = req.protocol = matches[4];

		// Fill headers
		for (size_t i = 0; i < matches.captures(5).size(); ++i)
		{
			req.headers[matches.captures(5)[i]] = matches.captures(6)[i];
		}

		// Fill Params
		if (matches[3].matched)
		{
			req.queryString = matches[3];
			boost::regex e("(?:([^\\s=&;]+)=?([^\\s=&;]*)[&;]*)*");
			boost::smatch m;
			bool v = boost::regex_match(req.queryString, m, e, boost::match_extra);
			if (v)
			{
				for (size_t i = 0; i < m.captures(1).size(); ++i)
				{
					req.params[m.captures(1)[i]] = m.captures(2)[i];
				}
			}

		}

		if (req.method == "GET" || req.method == "HEAD")
		{
			res.status = 200;
			res.reason = "OK";
			return transit<StateReqProcess>();
		}
		else if(matches[1] == "POST")
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
	HTTP::Response& res = outermost_context().env.res;
	res.data << "<h1>" << res.status << " " << res.reason << "</h1>";
	res.headers["Content-Type"] = "text/html";
	res.headers["Content-Length"] = boost::lexical_cast<std::string>(res.data.tellp());
	post_event(EvtReqDone());
}

sc::result 
HTTP::StateRecvReqBody::react(const EvtReadComplete& evt)
{
	return discard_event();
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


HTTP::StateReqProcess::StateReqProcess(my_context ctx) : my_base(ctx)
{
	HTTP::Request& req = outermost_context().env.req;
	HTTP::Response& res = outermost_context().env.res;
	req.requestInfo(res.data);
	res.headers["Content-Type"] = "text/html";
	res.headers["Content-Length"] = boost::lexical_cast<std::string>(res.data.tellp());
	post_event(EvtReqDone());
}

/////////////////////////////////////////////////////////////////////////////////////
// Send States
/////////////////////////////////////////////////////////////////////////////////////

HTTP::StateSendResHeader::StateSendResHeader(my_context ctx) : my_base(ctx)
{
	Response& res = outermost_context().env.res;
	// Add p3p header
	res.headers["P3P"] = "CP=\"NON NID TAIa OUR NOR NAV INT STA\"";
	res.sendHeaders(outermost_context().conn);
}

sc::result 
HTTP::StateSendResHeader::react(const EvtWriteComplete& evt)
{
	return transit<StateSendResBody>();
}

HTTP::StateSendResBody::StateSendResBody(my_context ctx) : my_base(ctx)
{
	outermost_context().env.res.sendBody(outermost_context().conn);
}

sc::result 
HTTP::StateSendResBody::react(const EvtWriteComplete& evt)
{
	Request& req = outermost_context().env.req;
	if (!boost::ilexicographical_compare(req.headers["Connection"],"close") && req.protocol != "HTTP/1.0")
	{
		post_event(EvtKeepAlive());
	}
	else
	{
		outermost_context().conn.close();
		post_event(EvtClose());
	}
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







