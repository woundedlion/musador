#include <stdio.h>
#include <boost/filesystem.hpp>

#include "Server.h"
#include "Utilities/Util.h"
#include "Utilities/md5.h"
#include "Controller.h"
#include "boost/bind.hpp"
#include "HTTPListener.h"
#include "HTTPConnection.h"


#include "Logger/Logger.h"
#define LOG_SENDER L"Server"

using namespace Musador;
namespace fs = boost::filesystem;

//////////////////////////////////////////////////////////////////////////////////////
// Server
//////////////////////////////////////////////////////////////////////////////////////

// Constructor/Destructor
Server::Server(ServerConfig& cfg) :
net(Musador::Network::instance()),
doRecycle(false),
doShutdown(false),
running(false),
cfg(cfg)
{
}


Server::~Server() 
{
    net = NULL;
}

void 
Server::start() 
{
    LOG(Info) << "Server starting...";

    // Start listeners
    ServerConfig::HTTPSiteCollection sites = cfg.sites;
    for (ServerConfig::HTTPSiteCollection::const_iterator iter = sites.begin(); 
        iter != sites.end(); ++iter)
    {
        if (!fs::exists(iter->documentRoot.get()))
        {
            LOG(Error) << "Document Root does not exist: " << iter->documentRoot;
        }

        sockaddr_in ep = {0};
        ep.sin_family = AF_INET;
        ep.sin_addr.s_addr = ::inet_addr(iter->addr.get().c_str());
        ep.sin_port = ::htons(iter->port);

        HTTP::Env env;
        env.cfg.reset(new HTTPConfig(*iter));
        env.controller = cfg.controller;
        env.server = shared_from_this();

        boost::shared_ptr<IO::Listener> listener(new HTTPListener(ep));
        acceptConnections(listener, env);
    }

    // We're officially started now
    LOG(Info) << "Server started...";
    {
        Guard guard(runningMutex);
        running = true;
        runningCV.notify_all();
    }

}

void 
Server::waitForStart()
{
    Guard guard(runningMutex);
    while(!running)
    {
        runningCV.wait(guard);
    }
}

void 
Server::stop() 
{
    LOG(Info) << "Server shutting down...";

    // Shutting down...
    killConnections();

    for (ListenerCollection::iterator iter = listeners.begin(); iter != listeners.end(); ++iter)
    {
        try
        {	
            (*iter)->close();
        }
        catch (const NetworkException& e)
        {
            LOG(Warning) << e.what();
        }
    }

    {
        Guard guard(runningMutex);
        running = false;
        runningCV.notify_all();
    }

    LOG(Info) << "Server stopped...";
}

void 
Server::waitForStop()
{
    Guard guard(runningMutex);
    while(running)
    {
        runningCV.wait(guard);
    }
}

void 
Server::restart() 
{
    doRecycle = true;
}

void 
Server::acceptConnections(boost::shared_ptr<IO::Listener> listener, 
                          boost::any tag /* = NULL */)
{

    listeners.push_back(listener);

    // Do the async accept
    listener->beginAccept(boost::bind(&Server::onAcceptComplete,this,_1,_2), tag);
}


void 
Server::onAcceptComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag)
{
    assert(msg->getType() == IO::MSG_SOCKET_ACCEPT_COMPLETE);
    boost::shared_ptr<IO::MsgSocketAcceptComplete> msgAccept(boost::shared_static_cast<IO::MsgSocketAcceptComplete>(msg));
    if (msgAccept->isError())
    {
        LOG(Error) << "Accept failed on : " << msgAccept->listener->toString() << " : " << msgAccept->getError();
        // TODO: reschedule the accept on an error?
    }
    else
    {
        // Do another async accept
        msgAccept->listener->beginAccept(boost::bind(&Server::onAcceptComplete,this,_1,_2),tag);

        // Set up the new connection
        addConnection(msgAccept->conn);
    }
}

void 
Server::onError(boost::shared_ptr<IO::Connection> conn, const IO::Msg::ErrorCode& err)
{
    LOG(Info) << "Error detected on " << conn->toString() << ": " << err;
    killConnection(conn);
}


Session & 
Server::getSession(const std::string& key)
{
    Guard lock(sessionsMutex);
    if (NULL == sessions[key])
    {
        sessions[key].reset(new Session(key));
    }
    return *sessions[key];
}

void 
Server::addConnection(boost::shared_ptr<IO::Connection> conn)
{
    LOG(Info) << "Connected: " << conn->toString();
    Guard lock(connsMutex);
    conns.push_back(conn);
}

void 
Server::removeConnection(boost::shared_ptr<IO::Connection> conn)
{
    Guard lock(connsMutex);
    for (ConnCollection::iterator iter = conns.begin(); iter!= conns.end(); ++iter)
    {
        conns.erase(iter);
        break;
    }
    LOG(Info) << "Disconnected: " << conn->toString();
}

void 
Server::killConnection(boost::shared_ptr<IO::Connection> conn)
{
    removeConnection(conn);
}

void 
Server::killConnections()
{
    Guard lock(connsMutex);
    ConnCollection::iterator iter;
    conns.clear();
}


/*
int CALLBACK Server::authorizeIPWrapper(
IN LPWSABUF lpCallerId,
IN LPWSABUF ,
IN OUT LPQOS ,
IN OUT LPQOS ,
IN LPWSABUF ,
OUT LPWSABUF ,
OUT GROUP FAR *,
IN DWORD_PTR dwCallbackData ) 
{
Server * server = (Server *)dwCallbackData;
if (server->authorizeIP(inet_ntoa(((SOCKADDR_IN *)(lpCallerId->buf))->sin_addr)))
return CF_ACCEPT;
else
return CF_REJECT;
}

bool Server::authorizeIP(char * ip) {
bool allowed = false;
bool denied = false;
vector<string>::iterator iter;
vector<string> ipOctets, iterOctets;
Util::tokenize(ip,ipOctets,".");
int i;
if (ipOctets.size() != 4) // bad ip 
return false; //deny

// Check against Allow
for (iter = IPAllow.begin(); iter < IPAllow.end() && !allowed; iter++) {
iterOctets.clear();
Util::tokenize(*iter,iterOctets,".");
if (iterOctets.size() < 4) // bad ip 
continue;
denied = false;
for (i = 0; i < 4 && !denied; i++) {
if (ipOctets[i] == iterOctets[i] || iterOctets[i] == "*") 
continue;	// octet passed
else
denied = true;
}
allowed = !denied;
}

// Check Against Deny
denied = false;
for (iter = IPDeny.begin(); iter < IPDeny.end() && !denied; iter++) {
iterOctets.clear();
Util::tokenize(*iter,iterOctets,".");
if (iterOctets.size() < 4) // bad ip 
continue;
denied = true;
for (i = 0; i < 4; i++) {
if (ipOctets[i] == iterOctets[i] || iterOctets[i] == "*") 
continue;	// octet passed
else
denied = false;
}
}
return allowed && !denied;
}
*/

//////////////////////////////////////////////////////////////////////////////////////
// RequestThread
//////////////////////////////////////////////////////////////////////////////////////
/*

int RequestThread::run() {
request = new Request();
do {
request->receiveFrom(clientSocket);
setLastRequestURI(request->requestURI);

if (request->status == -1)
break;

Response * response = new Response();
// Add p3p header
response->headers["P3P"] = "CP=\"NON NID TAIa OUR NOR NAV INT STA\"";

response->remoteSocket = clientSocket;

// TODO: Cleanup/expire sessions
// Create or retrieve session
string realm = server->getAuthRealm(request->requestURI);
CookieStore cookies(request->headers["Cookie"]);
string sessionKey = cookies[realm.c_str()];
if (sessionKey.empty()) {
sessionKey = request->params[realm];
if (sessionKey.empty()) {
sessionKey = request->params["auth"];
if (sessionKey.empty()) {
Util::genGUID(sessionKey);
cookies[realm] = sessionKey;
response->headers["Set-Cookie"] = realm + "=" + sessionKey + ";";
}
}
}
session = server->getSession(sessionKey);
sessionName = realm.c_str();

// Send 401 if necessary
if (server->getRequireAuth() && request->status != 500) {
if (!server->authorize(request,session)) {	
//				response->headers["WWW-Authenticate"] = "Basic realm=\"AjaxAMP\"";
HTTP::Util::genDigestNonce((*session)["nonce"]);
if ((*session)["opaque"].empty())
HTTP::Util::genDigestOpaque((*session)["opaque"]);
response->headers["WWW-Authenticate"] = "Digest realm=\"" + realm + "\" , qop=\"auth\" , nonce=\"" + (*session)["nonce"] + "\" , opaque=\"" + (*session)["opaque"] + "\"\r\nWWW-Authenticate: Basic realm=\"AjaxAMP\"";
request->status = 401;
request->reason = "Unauthorized";
} else {
request->authString = sessionKey;
}
} 

if (request->headers["Connection"] == "close" || request->status == 500 || request->status == 401)
response->headers["Connection"] = "close";
if (request->status) {
response->status = request->status; 
response->reason = request->reason; 
response->sendResponse();
} else if (!(request->headers["If-Modified-Since"].empty())) {
response->status = 304;
response->reason = "Not Modified";
response->sendResponse();
} else {
handleRequest(response);
}	
delete response;
server->notifyConnectionListeners();
} while (request->headers["Connection"] != "close" && request->status == 0);
delete request;
return 0;
}

*/
