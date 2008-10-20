#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "HTTPConnection.h"
#include "HTTP.h"
#include "boost/regex.hpp"
#include "Logger/Logger.h"
#include <time.h>
#include "Controller.h"
#include "Server.h"
#include "Utilities/MIMEResolver.h"

#define LOG_SENDER L"HTTPConnection"


using namespace Musador;
namespace fs = boost::filesystem;

HTTPConnection::HTTPConnection() :
#pragma warning(push)
#pragma warning(disable:4355)
fsm(*this)
#pragma warning(pop)
{
    this->fsm.initiate();
}

HTTPConnection::~HTTPConnection()
{
    this->fsm.terminate();
}

void 
HTTPConnection::onAcceptComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /*= NULL*/)
{
    this->env = boost::any_cast<HTTP::Env>(tag);
    this->env.req = &this->fsm.req;
    this->env.res = &this->fsm.res;

    Guard lock(this->fsmMutex);
    this->fsm.process_event(HTTP::EvtOpen());
}

void 
HTTPConnection::onReadComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /*= NULL*/)
{
    switch (msg->getType())
    {
    case IO::MSG_READ_COMPLETE:
        {
            boost::shared_ptr<IO::MsgReadComplete> & msgRead = boost::shared_static_cast<IO::MsgReadComplete>(msg);
            Guard lock(this->fsmMutex);
            this->fsm.process_event(HTTP::EvtReadComplete(msgRead));
        }
        break;

    case IO::MSG_ERROR:
        {
            this->env.server->onError(boost::shared_static_cast<IO::MsgError>(msg));
        }
        break;
    }
}

void 
HTTPConnection::onWriteComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /*= NULL*/)
{
    switch (msg->getType())
    {
    case IO::MSG_WRITE_COMPLETE:
        {
            boost::shared_ptr<IO::MsgWriteComplete> & msgWrite = boost::shared_static_cast<IO::MsgWriteComplete>(msg);
            Guard lock(this->fsmMutex);
            this->fsm.process_event(HTTP::EvtWriteComplete(msgWrite));
        }
        break;
    case IO::MSG_ERROR:
        {
            this->env.server->onError(boost::shared_static_cast<IO::MsgError>(msg));
        }
        break;
    }
}

inline
HTTP::Env&
HTTPConnection::getEnv()
{
    return this->env;
}

/////////////////////////////////////////////////////////////////////////////////////
// HTTP Protocol State machine logic
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// Receive States
/////////////////////////////////////////////////////////////////////////////////////

HTTP::StateRecvReqHeader::StateRecvReqHeader(my_context ctx) : my_base(ctx)
{
    outermost_context().req.clear();
    outermost_context().res.clear();
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
        Request& req = outermost_context().req;
        Response& res = outermost_context().res;
        Env& env = outermost_context().conn.getEnv();

        // Fill the request object
        req.method = matches[1];
        req.requestURI = matches[2];
        res.protocol = req.protocol = matches[4];

        // Fill headers
        for (size_t i = 0; i < matches.captures(5).size(); ++i)
        {
            req.headers[matches.captures(5)[i]] = matches.captures(6)[i];
        }

        // Fill Cookies
        HTTP::parseCookie(req.headers["Cookie"],req.cookies);

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

        // TODO: Cleanup/expire sessions
        // Set up Session
        std::string sessionName = Util::unicodeToUtf8(env.cfg->realm);
        if (sessionName.empty())
        {
            // TODO: generate unique sessionName?
            sessionName = "Restricted";
        }
        std::string sessionKey(req.cookies[sessionName]);
        if (sessionKey.empty())
        {
            Util::genGUID(sessionKey);
        }
        res.headers["Set-Cookie"] = sessionName + "=" + sessionKey + ";";
        env.session = &env.server->getSession(sessionKey);

        // Handle Auth
        if (env.cfg->requireAuth) 
        {
            if (!HTTP::auth(env)) 
            {	
                env.session->set("nonce",HTTP::genDigestNonce());
                if (env.session->get<std::string>("opaque").empty())
                {
                    env.session->set("opaque",HTTP::genDigestOpaque());
                }
                res.headers["WWW-Authenticate"] = "Digest realm=\"" + sessionName + "\" , qop=\"auth\" , nonce=\"" + 
                    env.session->get<std::string>("nonce") + "\" , opaque=\"" + env.session->get<std::string>("opaque") + "\"\r\n";
                //				res.headers["WWW-Authenticate"] = "Basic realm=\"" + sessionName + "\" ";
                res.status = 401;
                res.reason = "Unauthorized";
                return transit<StateReqError>();
            }
        } 		

        // dispatch
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
        if (evt.msgRead->MAX == evt.msgRead->len)
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
    res.data.reset(new std::stringstream());
    *res.data << "<h1>Error " << res.status << " " << res.reason << "</h1><br/>";
    *res.data << "<i>" << HTTP::getRFC1123(::time(NULL)) << "</i>";
    res.headers["Content-Type"] = "text/html";
    res.headers["Content-Length"] = boost::lexical_cast<std::string>(res.data->tellp());
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
    Request& req = outermost_context().req;
    Response& res = outermost_context().res;
    Env& env = outermost_context().conn.getEnv();

    // If the request exists in the file system, serve that
    fs::wpath fname(env.cfg->documentRoot);
    fname /= Util::utf8ToUnicode(req.requestURI);
    if (fs::exists(fname))
    {
        if(fs::is_directory(fname))
        {
            if (fs::exists(fname / L"index.html"))
            {
                fname /= L"index.html";
                if (!this->sendFile(env,fname.file_string()))
                {
                    post_event(EvtReqError());
                    return;		
                }
            }
            else
            {
                if (!this->dirIndex(env,fname.directory_string()))
                {
                    post_event(EvtReqError());
                    return;		
                }
            }
        }
        else if(fs::is_regular(fname))
        {
            if (!this->sendFile(env,fname.file_string()))
            {
                post_event(EvtReqError());
                return;		
            }
        }

        post_event(EvtReqDone());
        return;		
    }

    // otherwise try to map the request to a command
    if (env.controller)
    {
        try 
        {
            if (env.controller->exec(env))
            {
                post_event(EvtReqDone());
                return;
            }
        }
        catch (const std::exception& e)
        {
            res.status = 500;
            res.reason = "Internal Server Error: ";
            res.reason += e.what();
            post_event(EvtReqError());
            return;		
        }
    }

    // otherwise 404
    res.status = 404;
    res.reason = "Not Found";
    post_event(EvtReqError());
}

bool 
HTTP::StateReqProcess::sendFile(HTTP::Env& env, const std::wstring& path)
{
    Response& res = outermost_context().res;

    env.res->data.reset(new std::fstream(path.c_str(), std::ios::in | std::ios::binary));
    if (env.res->data->fail())
    {
        res.status = 500;
        res.reason = "Internal Server Error";
        return false;
    }

    env.res->headers["Accept_ranges"] = "bytes";
    env.res->headers["Content-transfer-encoding"] = "binary";
    env.res->headers["Content-Disposition"] = "inline; filename=\"";
    env.res->headers["Content-Disposition"] += Util::unicodeToUtf8(fs::wpath(path).leaf());
    env.res->headers["Content-Disposition"] += "\"";

    boost::uintmax_t fsize = fs::file_size(path);

    // handle seek
    if (!env.req->headers["Range"].empty()) {
        std::pair<std::string,std::string> unitRange;
        std::pair<std::string,std::string> offsetEnd;
        Util::parseNameValuePair(env.req->headers["Range"],'=',unitRange);
        Util::parseNameValuePair(unitRange.second,'-',offsetEnd);

        env.res->data->seekg(boost::lexical_cast<int>(offsetEnd.first));
        env.res->status = 206;
        env.res->reason = "Partial Content";
        env.res->headers["Content-Range"] = "bytes ";
        env.res->headers["Content-Range"] += offsetEnd.first;
        env.res->headers["Content-Range"] += "-/";
        env.res->headers["Content-Range"] += boost::lexical_cast<std::string>(fsize);
        env.res->headers["Content-Length"] = boost::lexical_cast<std::string>(fsize - boost::lexical_cast<int>(offsetEnd.first));
    }
    else
    {
        res.status = 200;
        res.reason = "OK";
        res.headers["Content-Length"] = boost::lexical_cast<std::string>(fs::file_size(path));
    }

    res.headers["Content-Type"] = Util::unicodeToUtf8(MIMEResolver::instance()->MIMEType(path));
    return true;
}

bool
HTTP::StateReqProcess::dirIndex(HTTP::Env& env, const std::wstring& path)
{
    env.res->status = 403;
    env.res->reason = "Forbidden";
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////
// Send States
/////////////////////////////////////////////////////////////////////////////////////

HTTP::StateSendResHeader::StateSendResHeader(my_context ctx) : my_base(ctx)
{
    Response& res = outermost_context().res;
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
    Request& req = outermost_context().req;
    Response& res = outermost_context().res;
    if (NULL != res.data)
    {
        outermost_context().res.sendBody(outermost_context().conn);
    }
    else
    {
        // Done writing the response, move on
        if (!boost::ilexicographical_compare(req.headers["Connection"],"close") && req.protocol != "HTTP/1.0")
        {
            post_event(EvtKeepAlive());
        }
        else
        {
            outermost_context().conn.close();
            post_event(EvtClose());
        }
    }
}

sc::result 
HTTP::StateSendResBody::react(const EvtWriteComplete& evt)
{
    Request& req = outermost_context().req;
    Response& res = outermost_context().res;
    if (!res.data->eof() && res.data->good())
    {
        return transit<StateSendResBody>();
    }

    // Done writing the response, move on
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

///////////////////////////////////////////////////////////////////////////
// HTTP FSM logic
///////////////////////////////////////////////////////////////////////////

HTTP::FSM::FSM(HTTPConnection& conn) :
conn(conn)
{

}








