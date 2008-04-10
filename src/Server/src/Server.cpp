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
Server::Server(const ServerConfig& cfg) :
net(Musador::Network::instance()),
doRecycle(false),
doShutdown(false),
running(false),
cfg(cfg)
{
}


Server::~Server() 
{
	this->net = NULL;
}

void 
Server::start() 
{
	LOG(Info) << "Server starting...";
	
	// Start listeners
	for (ServerConfig::HTTPSiteCollection::const_iterator iter = this->cfg.sites.begin(); 
		iter != this->cfg.sites.end(); ++iter)
	{
		if (!fs::exists(iter->documentRoot))
		{
			LOG(Error) << "Document Root does not exist: " << iter->documentRoot;
		}

		sockaddr_in ep = {0};
		ep.sin_family = AF_INET;
		ep.sin_addr.s_addr = ::inet_addr(iter->addr.c_str());
		ep.sin_port = ::htons(iter->port);

		HTTP::Env env;
		env.cfg = &*iter;
		env.controller = this->cfg.controller;
		env.server = this->shared_from_this();

		boost::shared_ptr<Listener> listener(new HTTPListener(ep));
		this->acceptConnections(listener, env);
	}

	// We're officially started now
	LOG(Info) << "Server started...";
	{
		Guard guard(this->runningMutex);
		this->running = true;
		this->runningCV.notify_all();
	}

}

void 
Server::waitForStart()
{
	Guard guard(this->runningMutex);
	while(!this->running)
	{
		this->runningCV.wait(guard);
	}
}

void 
Server::stop() 
{
	LOG(Info) << "Server shutting down...";

	// Shutting down...
	this->killConnections();

	for (ListenerCollection::iterator iter = this->listeners.begin(); iter != this->listeners.end(); ++iter)
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
		Guard guard(this->runningMutex);
		this->running = false;
		this->runningCV.notify_all();
	}

	LOG(Info) << "Server stopped...";
}

void 
Server::waitForStop()
{
	Guard guard(this->runningMutex);
	while(this->running)
	{
		this->runningCV.wait(guard);
	}
}

void 
Server::restart() 
{
    this->doRecycle = true;
}

void 
Server::acceptConnections(boost::shared_ptr<Listener> listener, 
                          boost::any tag /* = NULL */)
{

	this->listeners.push_back(listener);

	// Do the async accept
	listener->beginAccept(boost::bind(&Server::onAccept,this,_1,_2), tag);
}


void 
Server::onAccept(boost::shared_ptr<IOMsg> msg, boost::any tag)
{
	switch (msg->getType())
	{
	case IO_SOCKET_ACCEPT_COMPLETE:
		{
			boost::shared_ptr<IOMsgSocketAcceptComplete> msgAccept(boost::shared_static_cast<IOMsgSocketAcceptComplete>(msg));

			// Do another async accept
			msgAccept->listener->beginAccept(boost::bind(&Server::onAccept,this,_1,_2),tag);

			// Set up the new connection
			this->addConnection(msgAccept->conn);

			// Start the connection state machine
			msgAccept->conn->accepted(tag);
		}
		break;
	case IO_ERROR:
		{
// TODO: reschedule the accept on an error?
		}
		break;
	}
}

void 
Server::onError(boost::shared_ptr<IOMsgError> msgErr)
{
	// Kill the connection
	if (NULL != msgErr->err)
	{
		LOG(Info) << "Socket error detected on " << msgErr->conn->toString() << ": " << msgErr->err;
	}
	this->killConnection(msgErr->conn);
}


Session & 
Server::getSession(const std::string& key)
{
	Guard lock(this->sessionsMutex);
	if (NULL == this->sessions[key])
	{
		this->sessions[key].reset(new Session());
	}
	return *this->sessions[key];
}

void 
Server::addConnection(boost::shared_ptr<Connection> conn)
{
	LOG(Info) << "Connected: " << conn->toString();
	Guard lock(this->connsMutex);
	this->conns.push_back(conn);
}

void 
Server::removeConnection(boost::shared_ptr<Connection> conn)
{
	Guard lock(this->connsMutex);
	for (ConnCollection::iterator iter = this->conns.begin(); iter!= this->conns.end(); ++iter)
	{
		this->conns.erase(iter);
		break;
	}
	LOG(Info) << "Disconnected: " << conn->toString();
}

void 
Server::killConnection(boost::shared_ptr<Connection> conn)
{
	this->removeConnection(conn);
}

void 
Server::killConnections()
{
	Guard lock(this->connsMutex);
	ConnCollection::iterator iter;
	this->conns.clear();
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
	for (iter = this->IPAllow.begin(); iter < this->IPAllow.end() && !allowed; iter++) {
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
	for (iter = this->IPDeny.begin(); iter < this->IPDeny.end() && !denied; iter++) {
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
	this->request = new Request();
	do {
		request->receiveFrom(this->clientSocket);
		this->setLastRequestURI(request->requestURI);
		
		if (request->status == -1)
			break;

		Response * response = new Response();
		// Add p3p header
		response->headers["P3P"] = "CP=\"NON NID TAIa OUR NOR NAV INT STA\"";

		response->remoteSocket = this->clientSocket;

		// TODO: Cleanup/expire sessions
		// Create or retrieve session
		string realm = this->server->getAuthRealm(request->requestURI);
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
		this->session = this->server->getSession(sessionKey);
		this->sessionName = realm.c_str();

		// Send 401 if necessary
		if (this->server->getRequireAuth() && request->status != 500) {
			if (!this->server->authorize(request,this->session)) {	
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
		this->server->notifyConnectionListeners();
	} while (request->headers["Connection"] != "close" && request->status == 0);
	delete request;
	return 0;
}

*/
