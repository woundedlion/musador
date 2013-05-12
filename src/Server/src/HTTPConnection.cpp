#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "HTTPConnection.h"
#include "HTTP.h"
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
    assert(msg->getType() == IO::MSG_READ_COMPLETE);
    boost::shared_ptr<IO::MsgReadComplete> & msgRead = boost::shared_static_cast<IO::MsgReadComplete>(msg);
    if (msgRead->isError())
    {
        this->env.server->onError(msgRead->conn, msgRead->getError());
    }
    else
    {
        Guard lock(this->fsmMutex);
        this->fsm.process_event(HTTP::EvtReadComplete(msgRead));
    }
}

void 
HTTPConnection::onWriteComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag /*= NULL*/)
{
    assert(msg->getType() == IO::MSG_WRITE_COMPLETE);
    boost::shared_ptr<IO::MsgWriteComplete> & msgWrite = boost::shared_static_cast<IO::MsgWriteComplete>(msg);
    if (msgWrite->isError())
    {
        this->env.server->onError(msgWrite->conn, msgWrite->getError());
    }
    else
    {
        Guard lock(this->fsmMutex);
        this->fsm.process_event(HTTP::EvtWriteComplete(msgWrite));
    }
}

void
HTTPConnection::close()
{
    SocketConnection::close();
    if (NULL != this->env.server)
    {
        boost::shared_ptr<IO::Connection> conn = shared_from_this();
        this->env.server->onError(conn, 0);
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

HTTP::StateRecvReqHeader::StateRecvReqHeader(my_context ctx) : my_base(ctx)
{
    outermost_context().req.clear();
    outermost_context().res.clear();
    if (!outermost_context().recvdData.empty())
    {
        post_event(EvtReadComplete(boost::shared_ptr<IO::MsgReadComplete>()));
        return;
    }
    else
    {
        outermost_context().conn.beginRead();
    }
}

sc::result 
HTTP::StateRecvReqHeader::react(const HTTP::EvtReadComplete& evt)
{
    // convenience references
    Request& req = outermost_context().req;
    Response& res = outermost_context().res;
    Env& env = outermost_context().conn.getEnv();
    HTTPConnection& conn = outermost_context().conn;
    IO::BufferChain<char>& recvdData = outermost_context().recvdData;

    // append the new Buffer if there is one
    if (NULL != evt.msgRead)
    {
        recvdData.append(evt.msgRead->buf);
    }

    // try to parse a request
    size_t reqLen = 0;
    if (HTTP::parseRequest(recvdData, req, reqLen))
    {	
        recvdData.pop(reqLen);

        // Copy the protocol to the response
        res.protocol = req.protocol;

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
            if (NULL == env.session)
            {
                Util::genGUID(sessionKey);
                env.session = &env.server->getSession(sessionKey);
            }
            else
            {
                sessionKey = env.session->getKey();
            }
            res.headers["Set-Cookie"] = sessionName + "=" + sessionKey + ";";
        }
        else
        {
            env.session = &env.server->getSession(sessionKey);
        }


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
        else if(req.method == "POST")
        {
            res.status = 405;
            res.reason = "Method Not Supported";
            return transit<StateReqError>();
        }
        else if (req.method == "OPTIONS")
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
    else // Haven't received a full header yet
    {
        // Limit length of BufferChain to avoid trivial attack scenario
        if (recvdData.length() > IO::MsgReadComplete::MAX * 2)
        {
            recvdData.pop(IO::MsgReadComplete::MAX);
        }
        conn.beginRead();
        return discard_event();
    }
}

HTTP::StateRecvReqBody::StateRecvReqBody(my_context ctx) : my_base(ctx)
{

}

sc::result 
HTTP::StateRecvReqBody::react(const EvtReadComplete& evt)
{
    // TODO: implement POST handling
    return discard_event();
}

HTTP::StateRecvReqBodyChunk::StateRecvReqBodyChunk(my_context ctx) : my_base(ctx)
{
    // TODO: implement chunked request handling
}

sc::result 
HTTP::StateRecvReqBodyChunk::react(const EvtReadComplete& evt)
{
    // TODO: implement chunked request handling
    return discard_event();
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
                if (!this->sendFile(env, fname.wstring()))
                {
                    res.status = 403;
                    res.reason = "Forbidden";
                    post_event(EvtReqError());
                    return;		
                }
            }
            else
            {
                if (!this->dirIndex(env, fname.wstring()))
                {
                    res.status = 403;
                    res.reason = "Forbidden";
                    post_event(EvtReqError());
                    return;		
                }
            }
        }
        else if(fs::is_regular(fname))
        {
            if (!this->sendFile(env,fname.wstring()))
            {
                res.status = 403;
                res.reason = "Forbidden";
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
        return false;
    }

    env.res->headers["Accept_ranges"] = "bytes";
    env.res->headers["Content-transfer-encoding"] = "binary";
    env.res->headers["Content-Disposition"] = "inline; filename=\"";
    env.res->headers["Content-Disposition"] += Util::unicodeToUtf8(fs::path(path).filename().wstring());
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

    res.headers["Content-Type"] = Util::unicodeToUtf8(Util::MIMEResolver::instance()->MIMEType(path));
    return true;
}

bool
HTTP::StateReqProcess::dirIndex(HTTP::Env& env, const std::wstring& path)
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////
// Send States
/////////////////////////////////////////////////////////////////////////////////////

HTTP::StateSendRes::~StateSendRes()
{
    // delete the data istream on exit to free up resources
    outermost_context().res.data.reset();
}

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
        if (!boost::iequals(req.headers["Connection"],"close") && req.protocol != "HTTP/1.0")
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
    if (!boost::iequals(req.headers["Connection"],"close") && req.protocol != "HTTP/1.0")
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

HTTP::StateClosed::StateClosed(my_context ctx) : my_base(ctx)
{
    outermost_context().conn.close();
}

HTTP::FSM::FSM(HTTPConnection& conn) :
conn(conn)
{

}








