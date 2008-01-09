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

void HTTP::Request::sendHeaders(const Connection& conn) {
	std::stringstream reqData;
	reqData << method << " " << requestURI << "?" << queryString << " " << protocol << "\r\n";
	std::map<std::string,std::string>::iterator hdr;
	for (hdr = headers.begin(); hdr != headers.end(); ++hdr) {
		reqData << hdr->first << ": " << hdr->second << "\r\n";
	}
	reqData << "\r\n";
	sendRaw(conn, reqData.str().c_str(),reqData.tellp());
}

void HTTP::Request::sendRequest(const Connection& conn) {
	this->sendHeaders(conn);
	this->sendRaw(conn,this->data.c_str(),static_cast<int>(this->data.size()));
}

void HTTP::Request::sendRaw(const Connection& conn, const char * data, int size) {
	// write to connection
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

HTTP::Response::~Response() {

}

void HTTP::Response::setData(const std::string& responseData) 
{
	data = responseData;
	char contLen[sizeof(unsigned int)*8+1];
	headers["Content-Length"] = _itoa((int)(data.size()),contLen,10);
}

const std::string& HTTP::Response::getData() 
{
	return data;
}

void HTTP::Response::sendHeaders(const Connection& conn) 
{
	std::stringstream responseData;
	responseData << protocol << " " << status << " " << reason << "\r\n";
	std::map<std::string,std::string>::iterator hdr;
	for (hdr = headers.begin(); hdr != headers.end(); ++hdr) {
		responseData << hdr->first << ": " << hdr->second << "\r\n";
	}
	responseData << "\r\n";
	this->sendRaw(conn, responseData.str().c_str(),responseData.tellp());
}

void HTTP::Response::sendResponse(const Connection& conn) {
	this->sendHeaders(conn);
	this->sendRaw(conn, this->data.c_str(),(unsigned int)(this->data.size()));
}

void HTTP::Response::sendRaw(const Connection& conn, const char * data, unsigned int size) {
	// write to connection
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
