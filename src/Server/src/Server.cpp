#include "Server.h"
#include "stdio.h"
#include "Utilities/Util.h"
#include "Utilities/md5.h"
#include "ConnectionProcessor.h"
#include "boost/bind.hpp"
#include "Logger/Logger.h"

#define LOG_SENDER L"Server"

using namespace Musador;

//////////////////////////////////////////////////////////////////////////////////////
// Server
//////////////////////////////////////////////////////////////////////////////////////

// Constructor/Destructor
Server::Server() :
net(Musador::Network::instance()),
doRecycle(false),
doShutdown(false),
running(false)
{
}


Server::~Server() 
{
	this->net = NULL;
    Musador::Network::destroy();
	Musador::ConnectionProcessor::destroy();
}

void Server::start() 
{
	LOG(Info) << "Server starting...";
	boost::thread ioThread(boost::bind(&Server::runIO,this));
	Musador::ConnectionProcessor::instance();
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
	Proactor::instance()->doShutdown = true;
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

void Server::runIO()
{
	// We're officially started now
	LOG(Info) << "Server started...";
	{
		Guard guard(this->runningMutex);
		this->running = true;
		this->runningCV.notify_all();
	}

	// start IO engine
	Proactor::instance()->runIO();

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

	LOG(Info) << "Server stopped...";
	{
		Guard guard(this->runningMutex);
		this->running = false;
		this->runningCV.notify_all();
	}
}

template <class ConnType>
void Server::acceptConnections(
							   const sockaddr_in& localEP, 
							   int socketType /* = SOCK_STREAM */, 
							   int socketProto /* = IPPROTO_TCP */)
{
	// Set up server socket
	SOCKET s;
	try
	{
		s = net->socket(localEP.sin_family,socketType,socketProto);
		net->bind(s,const_cast<sockaddr_in *>(&localEP));
		net->listen(s);
		this->listeners[s] = boost::shared_ptr<ConnectionFactory>(new ConcreteFactory<Connection,ConnType>());
		Proactor::instance()->beginAccept(s,this->listeners[s],boost::bind(&Server::onAccept,this,_1,_2),s);
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
			// Listening socket was passed in the tag
			SOCKET s = boost::any_cast<SOCKET>(tag);
			Proactor::instance()->beginAccept(s,this->listeners[s],boost::bind(&Server::onAccept,this,_1,_2),s);

			// Set up the new connection
			msgAccept->conn->setErrorHandler(boost::bind(&Server::onError,this,_1));
			this->addConnection(msgAccept->conn);
			// Start the connection stat machine
			msgAccept->conn->accepted();
		}
		break;
	case IO_ERROR:
		{
			SOCKET s = boost::any_cast<SOCKET>(tag);
			Proactor::instance()->beginAccept(s,this->listeners[s],boost::bind(&Server::onAccept,this,_1,_2),s);
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
														int socketType /* = SOCK_STREAM */, 
														int socketProto /* = IPPROTO_TCP */);

#include "NullConnection.h"
template void Server::acceptConnections<NullConnection>(const sockaddr_in& localEP, 
														int socketType /* = SOCK_STREAM */, 
														int socketProto /* = IPPROTO_TCP */);

/*
bool Server::serveFile(Request * request, string path,stringMap args, Response * response) {
	UNREFERENCED_PARAMETER(request);
	Util::replaceChar(path,'/','\\');
	path = this->documentRoot + '\\' + path;
	WIN32_FIND_DATA found;
	if (this->isValidPath(path)) {
		HANDLE hFind = FindFirstFile(path.c_str(),&found);
		if (hFind == INVALID_HANDLE_VALUE) {
			return false;
		}
		::FindClose(hFind);
 	} else {
		return false;
	}
	// Stream file
	size_t pos;
	if ((pos = path.rfind('.')) != string::npos) { 
		string ext = path.substr(pos+1);
		// set MIMEType automaticaly if file extension is recognized
		if (Server::MIMETypes.find(ext) != Server::MIMETypes.end()) {
			string * MIMEType = &Server::MIMETypes[ext];
			response->headers["Content-Type"] = *MIMEType;
		} else { // Unknown MIMEType
		}
	}
	ifstream file;
	file.open(path.c_str(),ios::in | ios::binary);
	if (!file.is_open()) {
		response->status = 500;
		response->reason = "Internal Server Error";
		response->setData("<h1>500 Internal Server Error</h1>");
		response->sendResponse();
		return true;
	}
	response->headers["Accept_ranges"] = "bytes";
	response->headers["Content-transfer-encoding"] = "binary";
	response->headers["Content-Disposition"] = string("inline; filename=\"") + found.cFileName + "\"";
	char contLen[sizeof(int)*8+1];
	response->headers["Content-Length"] = _itoa( (found.nFileSizeHigh * MAXDWORD) + found.nFileSizeLow,contLen,10);

	// handle seek
	if (!request->headers["Range"].empty()) {
		stringPair unitRange;
		stringPair offsetEnd;
		Util::parseNameValuePair(request->headers["Range"],'=',unitRange);
		Util::parseNameValuePair(unitRange.second,'-',offsetEnd);
		
		file.seekg(atoi(offsetEnd.first.c_str()));
		response->status = 206;
		response->reason = "Partial Content";
		response->headers["Content-Range"] = string("bytes ") + offsetEnd.first + "-/" + _itoa((found.nFileSizeHigh * MAXDWORD) + found.nFileSizeLow,contLen,10);
		response->headers["Content-Length"] = _itoa( ((found.nFileSizeHigh * MAXDWORD) + found.nFileSizeLow) - atoi(offsetEnd.first.c_str()),contLen,10) ;
	} 

	response->sendHeaders();
	char buf[1024];
	while(!file.eof()) {
		file.read(buf,1024);
		response->sendRaw(buf,1024);
	}

	file.close();
	return true;
}

CB_RET Server::disposeRequest(CB_ARGS) {
	this->removeConnection((RequestThread *)args);
	delete (RequestThread *)args;
	this->notifyConnectionListeners();
	if (this->getConnectionCount() == 0)
		::SetEvent(this->allRequestsDone);
	return NULL;
}


/*

StateStore * Server::getSession(const string& key) {
	::EnterCriticalSection(&this->sessionsLock);
	StateStore * rv = this->sessions[key];
	if (rv == NULL) {
		rv = this->sessions[key] = new StateStore();
	}
	::LeaveCriticalSection(&this->sessionsLock);
	return rv;
}
*/
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

RequestThread::~RequestThread() {
	closesocket(clientSocket);
	::DeleteCriticalSection(&this->lock);
}

string RequestThread::getRemoteEP() {
	::EnterCriticalSection(&this->lock);
	string ep = this->remoteEP;
	::LeaveCriticalSection(&this->lock);
	return ep;
}

void RequestThread::setRemoteEP(const string& ep) {
	::EnterCriticalSection(&this->lock);
	this->remoteEP = ep;
	::LeaveCriticalSection(&this->lock);
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

string RequestThread::getLastRequestURI() {
	::EnterCriticalSection(&this->lock);
	string uri = this->lastRequestURI;
	::LeaveCriticalSection(&this->lock);
	return uri;
}

void RequestThread::setLastRequestURI(const string& uri) {
	::EnterCriticalSection(&this->lock);
	this->lastRequestURI = uri;
	::LeaveCriticalSection(&this->lock);	
}

bool RequestThread::stop(){
	::shutdown(this->clientSocket,SD_SEND);
	return true;
}
*/
