#include "Utilities/Util.h"
#include "HTTP.h"
#include "Utilities/md5.h"

using namespace Musador;

///////////////////////////////////////////////////////////////////////////////////////////
/// HTTP
///////////////////////////////////////////////////////////////////////////////////////////
namespace
{
	MTRand randGen;
}

void HTTP::urlDecode(std::string& enc) {
	std::string::iterator iter;
	char x;
	for (iter = enc.begin(); iter < enc.end(); iter++) {
		if (*iter == '%' && iter < (enc.end() - 2) && Util::hexToDec(*(iter+1)) >= 0 && Util::hexToDec(*(iter+2)) >= 0) {
			x = Util::hexToChar(std::string(iter+1,iter+3));
			enc.replace(iter,iter+3,1,x);
		}
	}
}

void HTTP::urlDecode(std::pair<std::string,std::string>& pair) {
	HTTP::urlDecode(pair.first);
	HTTP::urlDecode(pair.second);
}

std::string HTTP::urlEncode(const std::string& enc) {
	std::string result = "";
	std::string::iterator iter;
	char code[4] = "%";
	for (std::string::const_iterator iter = enc.begin(); iter < enc.end(); iter++) {
		if (!isalpha((unsigned char)*iter) && !isdigit((unsigned char)*iter)) {
			if ((unsigned char)*iter < 16) {
				code[1]='0';
				_itoa((unsigned char)*iter,code+2,16);
			} else
				_itoa((unsigned char)*iter,code+1,16);
			result += code; 
		} else 
			result.push_back(*iter);
	}
	return result;
}

std::string * HTTP::getRFC1123(time_t timer, std::string * out) {
	time(&timer);
	tm * ptm = gmtime(&timer);
	char * days[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
	char * months[] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
	char year[5];
	char day[3];
	char hour[3];
	char min[3];
	char sec[3];
	
	*out = std::string(days[ptm->tm_wday]) + ", " + _itoa(ptm->tm_mday,day,10) + " " + months[ptm->tm_mon] + " " + _itoa(ptm->tm_year+1900,year,10) + " " + _itoa(ptm->tm_hour+1,hour,10) + ":" + _itoa(ptm->tm_min,min,10) + ":" + _itoa(ptm->tm_sec,sec,10) + " GMT";	
	return out;
}

void HTTP::genDigestOpaque(std::string& opaque) {
	char buf[17];
	::sprintf_s(buf,sizeof(buf),"%08x%08x",randGen.randInt(),randGen.randInt());
	buf[16] = 0;
	opaque = buf;
}

void HTTP::genDigestNonce(std::string& nonce, time_t timestamp) {
	md5_byte_t digest[16]; // md5( timestamp | private_key )
	char nonceBuf[16 + 32 + 1]; // timestamp | md5( timestamp | private_key )
	::sprintf_s(nonceBuf, sizeof(nonceBuf), "%016I64x%s", timestamp, HTTP_DIGEST_KEY);
	nonceBuf[48] = 0;
	md5_state_t state;
	md5_init(&state);
	md5_append(&state,(const md5_byte_t *)nonceBuf,sizeof(nonceBuf));
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di)
	    sprintf(nonceBuf + 16 + (di * 2), "%02x", digest[di]);	
	nonce = nonceBuf;
}

void HTTP::genDigestNonce(std::string& nonce) {
	time_t timer;
	::time(&timer);
	HTTP::genDigestNonce(nonce,timer);
}

void HTTP::genDigestResponse(std::string& response, std::map<std::string, std::string>& authInfo, const std::string& method, const std::string& password) {
	md5_state_t state;
	char digestBuf[33];
	digestBuf[32] = 0;

	md5_byte_t ha1[16];
	// calculate HA1
	md5_init(&state);
	md5_append(&state, (md5_byte_t *)(authInfo["username"].c_str()),static_cast<int>(authInfo["username"].size()));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(authInfo["realm"].c_str()),static_cast<int>(authInfo["realm"].size()));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(password.c_str()),static_cast<int>(password.size()));
	md5_finish(&state, ha1);

	md5_byte_t ha2[16];
	// calculate HA2
	md5_init(&state);
	md5_append(&state, (md5_byte_t *)(method.c_str()),static_cast<int>(method.size()));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(authInfo["uri"].c_str()),static_cast<int>(authInfo["uri"].size()));
	md5_finish(&state, ha2);

	md5_byte_t responseDigest[16];
	// calculate response
	md5_init(&state);
	for (int di = 0; di < 16; ++di)
	    sprintf(digestBuf + (di * 2), "%02x", ha1[di]);	
	md5_append(&state, (md5_byte_t *)digestBuf, static_cast<int>(::strlen(digestBuf)));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(authInfo["nonce"].c_str()), static_cast<int>(authInfo["nonce"].size()));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(authInfo["nc"].c_str()), static_cast<int>(authInfo["nc"].size()));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(authInfo["cnonce"].c_str()), static_cast<int>(authInfo["cnonce"].size()));
	md5_append(&state, (md5_byte_t *)":",1);
	md5_append(&state, (md5_byte_t *)(authInfo["qop"].c_str()), static_cast<int>(authInfo["qop"].size()));
	md5_append(&state, (md5_byte_t *)":",1);
	for (int di = 0; di < 16; ++di)
	    sprintf(digestBuf + (di * 2), "%02x", ha2[di]);	
	md5_append(&state, (md5_byte_t *)digestBuf, static_cast<int>(::strlen(digestBuf)));
	md5_finish(&state, (md5_byte_t *)responseDigest);

	// format response
	for (int di = 0; di < 16; ++di)
	    sprintf(digestBuf + (di * 2), "%02x", responseDigest[di]);	

	response = digestBuf;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Request
///////////////////////////////////////////////////////////////////////////////////////////

HTTP::Request::Request() {
	protocol = "HTTP/1.1";
}

HTTP::Request::~Request() {
}

void HTTP::Request::receiveFrom(SOCKET remoteSocket) {
	this->remoteSocket = remoteSocket;
	this->status = 500;
	this->reason = "Internal Server Error";
	this->headers.clear();
	this->params.clear();
	this->request.clear();
	this->requestURI.clear();
	this->authString.clear();

	// recieve the request into request buffer
	char req[MAXREQSIZE+1];
	size_t pos = 0;
	int rcvd = 0;
	bool foundTerm = false;
	do {
		rcvd = recv(remoteSocket,req+pos,(int)(MAXREQSIZE-pos),0);
		if (rcvd == SOCKET_ERROR) {
			//int error = WSAGetLastError();
			this->status = -1;
			return;
		}
		pos += rcvd;
		if (strstr(req,HTTP_TERM))
			foundTerm = true;
	} while(rcvd > 0 && pos < MAXREQSIZE && !foundTerm );
	if (pos == 0) {
		return;
	}
	req[pos] = 0;
	request = req;

	// parse the request line
	size_t pos2 = 0;
	pos = 0;
	size_t lineEnd;
	std::string token;
	lineEnd = request.find("\r\n");
	if (lineEnd == std::string::npos) {
		status = 400; // Bad Request
		reason = "Bad Request";
		return;
	}
	std::string reqLine = request.substr(0,lineEnd); 

	// Parse Method
	pos2 = reqLine.find(' ',pos);
	if (pos2 == std::string::npos) {
		status = 501; // Method not implemented;
		reason = "Method not implemented";
		return;
	}
	token = reqLine.substr(pos,pos2-pos);
	if (token == "GET")
		method = GET;
	else if (token == "POST")
		method = POST;
	else {
		status = 501; // Method not implemented;
		reason = "Method not implemented";
		return;
	}
	pos = ++pos2;

	// Parse URL
	pos2 = reqLine.find(' ',pos);
	token = reqLine.substr(pos,pos2-pos);
	if (token.empty() || token[0] != '/') { // Bad URL
		status = 400; // Bad Request
		reason = "Bad Request";
		return;
	}
	bool hasQS = false;
	if (token.find('?') != std::string::npos) { // check for querystring
		pos2 = reqLine.find('?',pos);
		requestURI = reqLine.substr(pos,pos2-pos);
		hasQS = true;
	} else {
		requestURI = token;
	}
	HTTP::urlDecode(requestURI);
	pos = ++pos2;

	// Parse Query String
	if (hasQS) {
		pos2 = reqLine.find(' ',pos);
		if (pos2 == std::string::npos) {
			status = 400; // Bad Request
			reason = "Bad Request";
			return;
		}
		token = reqLine.substr(pos,pos2-pos);
		queryString = token;
		pos = ++pos2;
	}

	// Parse Protocol
	pos2 = lineEnd;
	if (pos2 > pos) {
		token = reqLine.substr(pos,pos2-pos);
		protocol = token;
		pos2 += 2;
		pos = pos2;
	}

	// Parse Headers
	std::pair<std::string,std::string> nameValuePair;
	size_t termPos = request.find(HTTP_TERM,pos);
	while ((pos2 < termPos) && (pos2 = request.find("\r\n",pos)) != std::string::npos) {
		token = request.substr(pos,pos2-pos);
		if (Util::parseNameValuePair(token,':',nameValuePair))
			headers.insert(nameValuePair);
		pos2 += 2;
		pos = pos2;
	}

	// parse data if POST
	if (method == POST) {
		pos+=2;
		unsigned int dataLength = atoi(headers["Content-Length"].c_str());
		if ((request.length() - pos) < dataLength) { // more request to rcv
			size_t dataPos = 0;
			do {
				rcvd = recv(remoteSocket,req+dataPos,MAXREQSIZE-(int)dataPos,0);
				if (rcvd == SOCKET_ERROR) {
					//int error = WSAGetLastError();
					this->status = -1;
					return;
				}
				dataPos += rcvd;
			} while(rcvd > 0 && dataPos < dataLength - (request.length() - pos) && dataPos < MAXREQSIZE);
			request.append(req);
		}
		pos2 = pos + atoi(headers["Content-Length"].c_str());
		data = request.substr(pos,pos2-pos);
//		if (true) { //maybe check Content-Type header here?
			pos = 0;
			while ((pos2 = data.find("&",pos)) != std::string::npos) {
				token = data.substr(pos,pos2-pos);
				if (Util::parseNameValuePair(token,'=',nameValuePair)) {
					HTTP::urlDecode(nameValuePair);
					params.insert(nameValuePair);
				}
				pos = ++pos2;
			}
			if (pos < data.length()) { // get last param without trailing ampersand
				token = data.substr(pos);
				if (Util::parseNameValuePair(token,'=',nameValuePair)) {
					HTTP::urlDecode(nameValuePair);
					params.insert(nameValuePair);
				}
			}			
//		}
	}

	// Parse Parameters
                                                                                                                                                                                                                	pos = 0;
	while ((pos2 = queryString.find("&",pos)) != std::string::npos) {
		token = queryString.substr(pos,pos2-pos);
		if (Util::parseNameValuePair(token,'=',nameValuePair)) {
			HTTP::urlDecode(nameValuePair);
			params.insert(nameValuePair);
		}
		pos = ++pos2;
	}
	if (pos < queryString.length()) { // get last param without trailing ampersand
		token = queryString.substr(pos);
		if (Util::parseNameValuePair(token,'=',nameValuePair)) {
			HTTP::urlDecode(nameValuePair);
			params.insert(nameValuePair);
		}
	}

	// All good, return
	this->status = 0;
	this->reason = "";
}

void HTTP::Request::requestInfo(std::stringstream &info) {
	info	<< "<table border=\"0\" cellspacing=\"2\">\r\n"
				<< "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Request:</b></td><td valign=\"top\"><pre>" 
				<< request 
				<< "</pre></td></tr>"
				<< "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Request URI:</b></td><td valign=\"top\"><pre>" 
				<< requestURI 
				<< "</pre></td></tr>"
				<< "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Protocol:</b></td><td valign=\"top\"><pre>" 
				<< protocol 
				<< "</pre></td></tr>"
			;
	// headers
	info << "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Headers:</b></td><td valign=\"top\"><table border=\"0\">";
	for (std::map<std::string,std::string>::iterator iter = headers.begin(); iter != headers.end(); iter++) {
		info << "<tr><td valign=\"top\"><i>" << iter->first << ":</i></td><td valign=\"top\">" << iter->second << "</td>";
	}
	info << "</table></td></tr>"; 
	// params
	info << "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Params:</b></td><td valign=\"top\"><table border=\"0\">";
	for (std::map<std::string,std::string>::iterator iter = params.begin(); iter != params.end(); iter++) {
		info << "<tr><td valign=\"top\"><i>" << iter->first << ":</i></td><td valign=\"top\">" << iter->second << "</td>";
	}
	info << "</table></td></tr>"; 
	// data
	info << "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Data:</b></td><td valign=\"top\"><pre>" 
			 << data 
			 << "</pre></td></tr>";
	
	// close table
	info << "</table>";
}

int HTTP::Request::sendHeaders() {
	std::stringstream reqData;
	std::string methodStr;
	if (method == Request::GET) 
		methodStr = "GET";
	else
		methodStr = "POST";
	reqData << methodStr << " " << requestURI << "?" << queryString << " " << protocol << "\r\n";
	std::map<std::string,std::string>::iterator hdr;
	for (hdr = headers.begin(); hdr != headers.end(); ++hdr) {
		reqData << hdr->first << ": " << hdr->second << "\r\n";
	}
	reqData << "\r\n";
	return sendRaw(reqData.str().c_str(),reqData.tellp());
}

int HTTP::Request::sendRaw(const char * data, int size) {
	int sent = 0;
	// write to socket
	sent = send(remoteSocket, data, size, 0);
	if(sent == SOCKET_ERROR) { return 0;}
	return sent;
}


int HTTP::Request::sendRequest() {
	return this->sendHeaders() + this->sendRaw(this->data.c_str(),(int)(this->data.size()));
}

///////////////////////////////////////////////////////////////////////////////
//// Response
///////////////////////////////////////////////////////////////////////////////

HTTP::Response::Response() {
	protocol = "HTTP/1.1";
	status = 200;
	reason = "OK";
	headers["Content-Type"] = "text/html";
}

void HTTP::Response::receiveFrom(SOCKET remoteSocket) {
	this->remoteSocket = remoteSocket;

	// recieve the response into buffer
	std::string response;
	char res[MAXREQSIZE+1];
	size_t pos = 0;
	int rcvd = 0;
	bool foundTerm = false;
	do {
		rcvd = recv(remoteSocket,res+pos,(int)(MAXREQSIZE-pos),0);
		if (rcvd == SOCKET_ERROR) {
			//int error = WSAGetLastError();
			return;
		}
		pos += rcvd;
		if (strstr(res,HTTP_TERM))
			foundTerm = true;
	} while(rcvd > 0 && pos < MAXREQSIZE && !foundTerm );
	res[pos] = 0; // NULL Terminate
	response = res;

	// parse the response line
	size_t pos2 = 0;
	pos = 0;
	size_t lineEnd;
	std::string token;
	lineEnd = response.find("\r\n");
	if (lineEnd == std::string::npos) {
		return;
	}
	std::string resLine = response.substr(0,lineEnd); 

	// Parse protocol
	pos2 = resLine.find(' ',pos);
	if (pos2 == std::string::npos) { // Bad protocol
		return;
	}
	token = resLine.substr(pos,pos2-pos);
	protocol = token;
	pos = ++pos2;

	// Parse Status
	pos2 = resLine.find(' ',pos);
	token = resLine.substr(pos,pos2-pos);
	if (token.empty()) { // Bad Status
		return;
	}
	status = atoi(token.c_str());
	pos = ++pos2;

	// Parse Reason
	pos2 = lineEnd;
	if (pos2 > pos) {
		token = resLine.substr(pos,pos2-pos);
		reason = token;
		pos2 += 2;
		pos = pos2;
	}

	// Parse Headers
	std::pair<std::string,std::string> nameValuePair;
	size_t termPos = response.find(HTTP_TERM,pos);
	while ((pos2 < termPos) && (pos2 = response.find("\r\n",pos)) != std::string::npos) {
		token = response.substr(pos,pos2-pos);
		if (Util::parseNameValuePair(token,':',nameValuePair))
			headers.insert(nameValuePair);
		pos2 += 2;
		pos = pos2;
	}

	// parse message
	pos+=2;
	unsigned int dataLength = (unsigned int)atoi(headers["Content-Length"].c_str());
	
	if ((response.length() - pos) < dataLength) { // more response to rcv
		size_t dataPos;
		do {	
			dataPos = 0;
			do {
				rcvd = recv(remoteSocket,res+dataPos,MAXREQSIZE-(int)dataPos-1,0);
				if (rcvd == SOCKET_ERROR) {
					//int error = WSAGetLastError();
					return;
				}
				dataPos += rcvd;
			} while(rcvd > 0 && dataPos < dataLength && dataPos < (size_t)(MAXREQSIZE - 1));
			res[dataPos] = 0;
			response.append(res);
		} while(rcvd > 0 && dataPos < dataLength);
	}
	pos2 = pos + atoi(headers["Content-Length"].c_str());
	data = response.substr(pos,pos2-pos);
}

HTTP::Response::~Response() {

}

void HTTP::Response::setData(const std::string& responseData) {
	data = responseData;
	char contLen[sizeof(unsigned int)*8+1];
	headers["Content-Length"] = _itoa((int)(data.size()),contLen,10);
}

const std::string& HTTP::Response::getData() {
	return data;
}

int HTTP::Response::sendHeaders() {
	std::stringstream responseData;
	responseData << protocol << " " << status << " " << reason << "\r\n";
	std::map<std::string,std::string>::iterator hdr;
	for (hdr = headers.begin(); hdr != headers.end(); ++hdr) {
		responseData << hdr->first << ": " << hdr->second << "\r\n";
	}
	responseData << "\r\n";
	return sendRaw(responseData.str().c_str(),responseData.tellp());
}

int HTTP::Response::sendRaw(const char * data, unsigned int size) {
	int sent = 0;
	// write to socket
	sent = send(remoteSocket, data, size, 0);
	if(sent == SOCKET_ERROR) { /* DWORD err = GetLastError(); */ return 0;}
	return sent;
}


int HTTP::Response::sendResponse() {
	int h = this->sendHeaders();
	return h + this->sendRaw(this->data.c_str(),(unsigned int)(this->data.size()));
}

///////////////////////////////////////////////////////////////////////////////////////////
/// StateStore
///////////////////////////////////////////////////////////////////////////////////////////

HTTP::StateStore::StateStore() {
}

HTTP::StateStore::~StateStore() {
}

std::string& HTTP::StateStore::operator[](const std::string& key) {	
	Guard guard(this->lock);
	std::string& rv = this->store[key];
	return rv;
}

void HTTP::StateStore::clear() {
	Guard(this->lock);
	this->store.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
/// CookieStore
///////////////////////////////////////////////////////////////////////////////////////////


// TODO: escape semicolons at least if not urlencode
HTTP::CookieStore::CookieStore(const std::string& cookieStr) : StateStore() {
	std::vector<std::string> cookies;
	Util::tokenize(cookieStr,cookies,"; ");
	std::pair<std::string,std::string> pair;
	for (std::vector<std::string>::iterator c = cookies.begin(); c != cookies.end(); c++) {
		Util::parseNameValuePair(*c,'=',pair);
		this->store[pair.first] = pair.second;
	}
}

// TODO: escape semicolons at least if not urlencode
std::string HTTP::CookieStore::toString() {
	std::stringstream result;
	for (std::map<std::string,std::string>::iterator c = this->store.begin(); c != this->store.end(); c++) {
		result << c->first << " = " << c->second << "; ";
	}	
	return result.str();
}
