#include "HTTPConnection.h"
#include "HTTP.h"
#include "boost/regex.hpp"
#include "Logger/Logger.h"
#include "Connection.h"
#define LOG_SENDER L"HTTP"


using namespace Musador;

HTTPConnection::HTTPConnection() :
fsm(*this)
{

}

HTTPConnection::~HTTPConnection()
{

}

void HTTPConnection::accepted()
{
	this->fsm.initiate();
}

void HTTPConnection::operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead)
{

}

/*
void HTTPConnection::error(Connection& conn, int errCode, const char * errMsg)
{
	HTTP::Response res;
	res.status = errCode;
	res.reason = errMsg;
	res.data << "<h1>" << errCode << " " << errMsg << "</h1>";
	res.headers["Content-Type"] = "text/html";
	res.headers["Content-Length"] = boost::lexical_cast<std::string>(res.data.tellp());
	res.sendResponse(conn);
}
*/

HTTP::FSM::FSM(Connection& conn) :
conn(conn)
{

}

HTTP::StateRecvReqHeader::StateRecvReqHeader(my_context ctx) : my_base(ctx)
{
	this->outermost_context().conn.beginRead();
}

sc::result HTTP::StateRecvReqHeader::react(const HTTP::EvtRecvdReqHeader& evt)
{
	return discard_event();
}

sc::result HTTP::StateSendRes::react(const HTTP::EvtSentRes& evt)
{
	return discard_event();
}

sc::result HTTP::StateSendResHeader::react(const HTTP::EvtSentResHeader& evt)
{
	return discard_event();
}



/*
void HTTPConnection::StateRecvReqHeader::read(boost::shared_ptr<IOMsgReadComplete> msgRead) const
{
    const char * start = msgRead->buf.get() + msgRead->off;
    const char * end = msgRead->buf.get() + msgRead->len;
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
		this->post_event(EvtGotReqHeader());

		if (matches[1] == "GET")
		{
		}
		else if(matches[1] == "POST")
		{
		}
		else if (matches[1] == "HEAD")
		{
		}
		else if (matches[1] == "OPTIONS")
		{
		}
		else
		{
		}
    }
	else // Haven't received the full header yet, keep reading into the same message
	{
		msgRead->conn->beginRead(msgRead);
	}
}
*/

/*
void HTTPConnection::StateRecvReqData::read(boost::shared_ptr<IOMsgReadComplete> msgRead) const
{

}
*/





