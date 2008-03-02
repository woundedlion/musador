#include "Server.h"
#include "stdio.h"
#include "Utilities/Util.h"
#include "Utilities/md5.h"
#include "ConnectionProcessor.h"
#include "Controller.h"
#include "boost/bind.hpp"
#include "Logger/Logger.h"
#include "HTTPConnection.h"
#include <boost/filesystem.hpp>


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

void Server::start() 
{
	LOG(Info) << "Server starting...";
	
	// Start worker threads
	
	Proactor::instance()->start();
	this->processor.start();

	// Start listeners
	for (ServerConfig::SiteCollection::const_iterator iter = this->cfg.sites.begin(); 
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

		boost::shared_ptr<HTTP::Env> env(new HTTP::Env());
		env->cfg = &*iter;
		env->controller = this->cfg.controller;

		this->acceptConnections<HTTPConnection>(ep,boost::static_pointer_cast<ConnectionCtx>(env));
	}

	// We're officially started now
	LOG(Info) << "Server started...";
	{
		Guard guard(this->runningMutex);
		this->running = true;
		this->runningCV.notify_all();
	}

}

void Server::waitForStart()
{
	Guard guard(this->runningMutex);
	while(!this->running)
	{
		this->runningCV.wait(guard);
	}
}

void Server::stop() 
{
	LOG(Info) << "Server shutting down...";

	// Shutting down...
	this->killConnections();

	for (ListenerCollection::iterator iter = this->listeners.begin(); iter != this->listeners.end(); ++iter)
	{
		try
		{	
			net->closeSocket(iter->first);
			LOG(Info) << "Closing server socket " << iter->first;
		}
		catch (const NetworkException& e)
		{
			LOG(Warning) << e.what();
		}
	}

	this->processor.shutdown();

	{
		Guard guard(this->runningMutex);
		this->running = false;
		this->runningCV.notify_all();
	}

	Proactor::instance()->stop();
	Proactor::destroy();

	LOG(Info) << "Server stopped...";
}

void Server::waitForStop()
{
	Guard guard(this->runningMutex);
	while(this->running)
	{
		this->runningCV.wait(guard);
	}
}

void Server::restart() 
{
    this->doRecycle = true;
}

template <class ConnType>
void Server::acceptConnections(const sockaddr_in& localEP, 
							   boost::shared_ptr<ConnectionCtx> ctx /* = NULL */,
							   int socketType /* = SOCK_STREAM */, 
							   int socketProto /* = IPPROTO_TCP */)
{
	// Set up ctx;
	if (NULL == ctx)
	{
		ctx = boost::shared_ptr<ConnectionCtx>(new ConnectionCtx());
	}
	ctx->server = this;
	ctx->processor = &this->processor;

	// Set up server socket
	SOCKET s;
	try
	{
		s = net->socket(localEP.sin_family,socketType,socketProto);
		net->bind(s,const_cast<sockaddr_in *>(&localEP));
		net->listen(s);
		this->listeners[s] = boost::shared_ptr<ConnectionFactory>(new ConcreteFactory<Connection,ConnType>());
		Proactor::instance()->beginAccept(s,this->listeners[s],boost::bind(&Server::onAccept,this,_1,_2),ctx);
	}
	catch (const NetworkException& e)
	{
		LOG(Error) << e.what();
		return;
	}
	LOG(Debug) << "Accepting connections on " << ::inet_ntoa(localEP.sin_addr) << ":" << ::ntohs(localEP.sin_port) << " [" << s << "]";
}


void Server::onAccept(boost::shared_ptr<IOMsg> msg, boost::any tag)
{

	switch (msg->getType())
	{
	case IO_ACCEPT_COMPLETE:
		{
			boost::shared_ptr<IOMsgAcceptComplete> msgAccept(boost::shared_static_cast<IOMsgAcceptComplete>(msg));

			// Register for another accept notification
			SOCKET s = msgAccept->listener;
			Proactor::instance()->beginAccept(s,this->listeners[s],boost::bind(&Server::onAccept,this,_1,_2),tag);

			// Set up the new connection
			msgAccept->conn->setCtx(boost::any_cast<boost::shared_ptr<ConnectionCtx> >(tag));
			this->addConnection(msgAccept->conn);

			// Start the connection state machine
			msgAccept->conn->accepted();
		}
		break;
	case IO_ERROR:
		{
// TODO: reschedule the accept on an error?
		}
		break;
	}
}

void Server::onError(boost::shared_ptr<IOMsgError> msgErr)
{
	// Kill the connection
	if (NULL != msgErr->err)
	{
		LOG(Info) << "Socket error detected on socket " << msgErr->conn->getSocket() << ": " << msgErr->err;
	}
	this->killConnection(msgErr->conn);
}

void Server::addConnection(boost::shared_ptr<Connection> conn)
{
	LOG(Info) << "Connected: " << conn->toString();
	Guard guard(this->connsMutex);
	this->conns.push_back(conn);
}

void Server::removeConnection(boost::shared_ptr<Connection> conn)
{
	Guard guard(this->connsMutex);
	for (ConnCollection::iterator iter = this->conns.begin(); iter!= this->conns.end(); ++iter)
	{
		this->conns.erase(iter);
		break;
	}
	LOG(Info) << "Disconnected: " << conn->toString();
}

void Server::killConnection(boost::shared_ptr<Connection> conn)
{
	this->removeConnection(conn);
}

void Server::killConnections()
{
	Guard guard(this->connsMutex);
	ConnCollection::iterator iter;
	this->conns.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Template specializations

#include "HTTPConnection.h"
template void Server::acceptConnections<HTTPConnection>(const sockaddr_in& localEP, 
														boost::shared_ptr<ConnectionCtx> ctx /* = NULL */,
														int socketType /* = SOCK_STREAM */, 
														int socketProto /* = IPPROTO_TCP */);

#include "NullConnection.h"
template void Server::acceptConnections<NullConnection>(const sockaddr_in& localEP, 
														boost::shared_ptr<ConnectionCtx> ctx /* = NULL */,
														int socketType /* = SOCK_STREAM */, 
														int socketProto /* = IPPROTO_TCP */);

#include "EchoConnection.h"
template void Server::acceptConnections<EchoConnection>(const sockaddr_in& localEP, 
														boost::shared_ptr<ConnectionCtx> ctx /* = NULL */,
														int socketType /* = SOCK_STREAM */, 
														int socketProto /* = IPPROTO_TCP */);

/*

bool Server::authorize(Request * request, StateStore * session) {
	string& authString = request->headers["Authorization"];
	// Authorize from session
	if ((*session)["authorized"] == "true") {
		return true;
	}
	// Handle Basic HTTP Authentication
	else if (authString.substr(0,6) == "Basic ") { // Check auth in headers
		string authInfo = authString.substr(6,string::npos);
		// TODO: Handle multiple usernames
		// Must base64Decode and parse username then call:
		//if (!this->isValidUser(PARSED_USERNAME)
		//	return false;
		//string challenge = PARSED_USERNAME + ":" + this->getPassword(PARSED_USERNAME);
		string challenge = this->username + ":" + this->getPassword(this->username);
		Util::base64Encode(challenge);
		return (authInfo == challenge);
	}
	// Handle Digest HTTP Authentication
	else if (authString.substr(0,7) == "Digest ") {
		// Parse Auth Info
		vector<string> authTokens;
		Util::tokenize(authString.substr(7,string::npos),authTokens,", ");
		map<string,string> authInfo;
		for (vector<string>::iterator iter = authTokens.begin(); iter != authTokens.end(); iter++) {
			stringPair nameValuePair;
			if (Util::parseNameValuePair(*iter,'=',nameValuePair)) {
				if (nameValuePair.second.at(0) == '"')
					nameValuePair.second.erase(0,1);
				if (nameValuePair.second.at(nameValuePair.second.size()-1) == '"')
					nameValuePair.second.erase(nameValuePair.second.size()-1,1);
				authInfo[nameValuePair.first] = nameValuePair.second;
			}
		}

		// check opaque
		if (authInfo["opaque"] != (*session)["opaque"]) {
			return false;
		}

		// check uri
		unsigned int qs; 
		if (authInfo["uri"].substr(0,authInfo["uri"].find("?")) != request->requestURI.substr(0,request->requestURI.find("?")))
			return false;

		// check nonce
		string& nonce = authInfo["nonce"];
		string timestampStr = nonce.substr(0,16);
		char * timeStampStrEnd;
		time_t timestamp = ::_strtoi64(timestampStr.c_str(),&timeStampStrEnd,16);
		string challenge;
		HTTP::Util::genDigestNonce(challenge,timestamp);
		if (nonce != challenge)
			return false;

		// check cnonce

		// check nc

		// check username
		if (!this->isValidUser(authInfo["username"]))
			return false;

		// check response
		string& response = authInfo["response"];
		string methodStr;
		if (request->method == Request::GET) 
			methodStr = "GET";
		else
			methodStr = "POST";
		HTTP::Util::genDigestResponse(challenge,authInfo, methodStr, this->getPassword(authInfo["username"]));
		if (authInfo["response"] != challenge)
			return false;

		// authorized
		(*session)["authorized"] = "true";
		return true;
	} 
	return false;
}

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
RequestThread::RequestThread(SOCKET clientSocket, Server * server) {
	this->clientSocket = clientSocket;	
	this->server = server;
	::InitializeCriticalSection(&this->lock);
	sockaddr_in addr;
	int addrSize = sizeof(sockaddr_in);
	stringstream ep;
	if (SOCKET_ERROR == ::getpeername(this->clientSocket,(sockaddr *)&addr,&addrSize))
		ep << "?.?.?.?:???";
	else
		ep << ::inet_ntoa(addr.sin_addr) << ":" << ::ntohs(addr.sin_port);
	this->setRemoteEP(ep.str());

	this->server->addConnection(this);
	server->notifyConnectionListeners();

}

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
