#include "HTTPProtocol.h"
#include "HTTP.h"
#include "boost/regex.hpp"
#include "Logger/Logger.h"
#define LOG_SENDER L"HTTP"


using namespace Musador;

HTTPProtocol::HTTPProtocol() :
state(RECV_REQ_HEADER_STATE)
{

}

HTTPProtocol::~HTTPProtocol()
{

}

void HTTPProtocol::operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead)
{
    switch (this->state)
    {
    case RECV_REQ_HEADER_STATE:
        this->stateRecvReqHeader(msgRead);
        break;
    case RECV_REQ_DATA_STATE:
        break;
    }
}

void HTTPProtocol::stateRecvReqHeader(boost::shared_ptr<IOMsgReadComplete> msgRead)
{
    const char * start = msgRead->buf.get() + msgRead->off;
    const char * end = msgRead->buf.get() + msgRead->len;
    boost::regex expr("^(GET|POST|HEAD|OPTIONS)[[:s:]]+(/[^\\r\\n]*)[[:s:]]+HTTP/(1.[01])\\r\\n(([[:alnum:]\\-]+):[[:s:]]*([^\\r\\n]*))*\\r\\n"); 
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
        LOG(Debug) << matches.str();
    }

}
