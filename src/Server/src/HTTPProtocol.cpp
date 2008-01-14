#include "HTTPProtocol.h"
#include "HTTP.h"
#include "boost/regex.hpp"
#include "Logger/Logger.h"
#include "Connection.h"
#define LOG_SENDER L"HTTP"


using namespace Musador;

HTTPProtocol::HTTPProtocol() 
{

}

HTTPProtocol::~HTTPProtocol()
{

}

void HTTPProtocol::operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead)
{

}

void HTTPProtocol::error(Connection& conn, int errCode, const char * errMsg)
{
	HTTP::Response res;
	res.status = errCode;
	res.reason = errMsg;
	res.data << "<h1>" << errCode << " " << errMsg << "</h1>";
	res.headers["Content-Type"] = "text/html";
	res.headers["Content-Length"] = boost::lexical_cast<std::string>(res.data.tellp());
	res.sendResponse(conn);
}

void HTTPProtocol::readReqHeader(boost::shared_ptr<IOMsgReadComplete> msgRead)
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
		if (matches[1] == "GET")
		{
			this->error(*msgRead->conn, 405, "Method not supported");
			
		}
		else if(matches[1] == "POST")
		{
			this->error(*msgRead->conn, 405, "Method not supported");

		}
		else if (matches[1] == "HEAD")
		{
			this->error(*msgRead->conn, 405, "Method not supported");

		}
		else if (matches[1] == "OPTIONS")
		{
			this->error(*msgRead->conn, 405, "Method not supported");
		}
		else
		{
			this->error(*msgRead->conn, 405, "Method not supported");
		}
    }
}



