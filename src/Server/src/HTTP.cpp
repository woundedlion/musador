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

void 
HTTP::urlDecode(std::string& enc) {
	std::string::iterator iter;
	char x;
	for (iter = enc.begin(); iter < enc.end(); iter++) {
		if (*iter == '%' && iter < (enc.end() - 2) && Util::hexToDec(*(iter+1)) >= 0 && Util::hexToDec(*(iter+2)) >= 0) {
			x = Util::hexToChar(std::string(iter+1,iter+3));
			enc.replace(iter,iter+3,1,x);
		}
	}
}

void 
HTTP::urlDecode(std::pair<std::string,std::string>& pair) {
	HTTP::urlDecode(pair.first);
	HTTP::urlDecode(pair.second);
}

std::string 
HTTP::urlEncode(const std::string& enc) {
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

std::string 
HTTP::getRFC1123(const time_t& timer) {
	tm * ptm = gmtime(&timer);
	char * days[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
	char * months[] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

	char s[32];
	
	// Fri, 00 Feb 000000 00:00:00 GMT\0 // 32 bytes max
	::sprintf(s,"%s, %d %s %d %d:%02d:%02d GMT",
		days[ptm->tm_wday],
		ptm->tm_mday, 
		months[ptm->tm_mon], 
		ptm->tm_year+1900, 
		ptm->tm_hour+1, 
		ptm->tm_min, 
		ptm->tm_sec);	
	s[31] = '\0';
	return s;
}

std::string 
HTTP::genDigestOpaque() {
	char buf[17];
	::sprintf_s(buf,sizeof(buf),"%08x%08x",randGen.randInt(),randGen.randInt());
	buf[16] = 0;
	return buf;
}

std::string
HTTP::genDigestNonce(time_t timestamp) {
	md5_byte_t digest[16]; // md5( timestamp | private_key )
	char nonceBuf[16 + 32 + 1]; // timestamp | md5( timestamp | private_key )
	::sprintf_s(nonceBuf, sizeof(nonceBuf), "%016I64x%s", timestamp, HTTP_DIGEST_KEY);
	nonceBuf[48] = 0;
	md5_state_t state;
	md5_init(&state);
	md5_append(&state,(const md5_byte_t *)nonceBuf,sizeof(nonceBuf));
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di)
	{
	    sprintf(nonceBuf + 16 + (di * 2), "%02x", digest[di]);	
	}
	return nonceBuf;
}

std::string 
HTTP::genDigestNonce() {
	time_t timer;
	::time(&timer);
	return HTTP::genDigestNonce(timer);
}

std::string 
HTTP::genDigestResponse(std::map<std::string, std::string>& authInfo, const std::string& method, const std::string& password) {
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
	{
	    sprintf(digestBuf + (di * 2), "%02x", ha1[di]);
	}
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
	{
	    sprintf(digestBuf + (di * 2), "%02x", ha2[di]);	
	}
	md5_append(&state, (md5_byte_t *)digestBuf, static_cast<int>(::strlen(digestBuf)));
	md5_finish(&state, (md5_byte_t *)responseDigest);

	// format response
	for (int di = 0; di < 16; ++di)
	{
	    sprintf(digestBuf + (di * 2), "%02x", responseDigest[di]);
	}

	return digestBuf;
}

bool 
HTTP::auth(const Env& env)
{
	std::string& authString = env.req->headers["Authorization"];

	// Authorize from session
	if (env.session->get<bool>("authorized")) 
	{
		return true;
	}
	// Handle Basic HTTP Authentication
	else if (authString.substr(0,6) == "Basic ") 
	{ // Check auth in headers
		std::string authInfo = authString.substr(6);
		// TODO: Handle multiple usernames
		// Must base64Decode and parse username then call:
		//if (!this->isValidUser(PARSED_USERNAME)
		//	return false;
		//string challenge = PARSED_USERNAME + ":" + this->getPassword(PARSED_USERNAME);
		std::string challenge = std::string("gabe") + ":" + "gblgbl";
		Util::base64Encode(challenge);
		return (authInfo == challenge);
	}
	// Handle Digest HTTP Authentication
	else if (authString.substr(0,7) == "Digest ") 
	{
		// Parse Auth Info
		std::vector<std::string> authTokens;
		Util::tokenize(authString.substr(7),authTokens,", ");
		std::map<std::string,std::string> authInfo;
		for (std::vector<std::string>::iterator iter = authTokens.begin(); iter != authTokens.end(); iter++) 
		{
			std::pair<std::string, std::string> nameValuePair;
			if (Util::parseNameValuePair(*iter,'=', nameValuePair)) {
				if (nameValuePair.second.at(0) == '"')
					nameValuePair.second.erase(0,1);
				if (nameValuePair.second.at(nameValuePair.second.size()-1) == '"')
					nameValuePair.second.erase(nameValuePair.second.size()-1,1);
				authInfo[nameValuePair.first] = nameValuePair.second;
			}
		}

		// check opaque
		if (authInfo["opaque"] != env.session->get<std::string>("opaque") )
		{
			return false;
		}

		// check uri
		if (authInfo["uri"].substr(0,authInfo["uri"].find("?")) != env.req->requestURI.substr(0,env.req->requestURI.find("?")))
		{
			return false;
		}

		// check nonce
		std::string & nonce = authInfo["nonce"];
		std::string timestampStr = nonce.substr(0,16);
		char * timeStampStrEnd;
		time_t timestamp = ::_strtoi64(timestampStr.c_str(),&timeStampStrEnd,16);
		std::string challenge = HTTP::genDigestNonce(timestamp);
		if (nonce != challenge)
		{
			return false;
		}

		// check cnonce

		// check nc

		// check username
//		if (!this->isValidUser(authInfo["username"]))
//		{
//			return false;
//		}

		// check response
		std::string& response = authInfo["response"];
//		challenge = HTTP::genDigestResponse(authInfo, env.req->method, this->getPassword(authInfo["username"]));
		challenge = HTTP::genDigestResponse(authInfo, env.req->method, "gblgbl");
		if (authInfo["response"] != challenge)
		{
			return false;
		}

		// authorized
		env.session->set("authorized",true);
		return true;
	} 

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Request
///////////////////////////////////////////////////////////////////////////////////////////

HTTP::Request::Request() 
{
}

HTTP::Request::~Request() {
}

void 
HTTP::Request::clear()
{
	this->requestURI.clear();
	this->queryString.clear();
	this->protocol.clear();
	this->method.clear();
	this->params.clear();
	this->headers.clear();
	this->cookies.clear();
	this->data.reset();
}

void 
HTTP::Request::dump(std::ostream &info) {
	info	<< "<table border=\"0\" cellspacing=\"2\">\r\n"
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

	// cookies
	info << "<tr bgcolor=\"eeeeee\"><td valign=\"top\"><b>Cookies:</b></td><td valign=\"top\"><table border=\"0\">";
	for (std::map<std::string,std::string>::iterator iter = cookies.begin(); iter != cookies.end(); iter++) {
		info << "<tr><td valign=\"top\"><i>" << iter->first << ":</i></td><td valign=\"top\">" << iter->second << "</td>";
	}
	info << "</table></td></tr>"; 

	// close table
	info << "</table>";
}

void 
HTTP::Request::sendHeaders(Connection& conn) {
	std::stringstream reqData;
	reqData << method << " " << requestURI << "?" << queryString << " " << protocol << "\r\n";
	std::map<std::string,std::string>::iterator hdr;
	for (hdr = headers.begin(); hdr != headers.end(); ++hdr) {
		reqData << hdr->first << ": " << hdr->second << "\r\n";
	}
	reqData << "\r\n";

	conn.beginWrite(reqData);
}

void 
HTTP::Request::sendBody(Connection& conn) {
	conn.beginWrite(*(this->data));
}


///////////////////////////////////////////////////////////////////////////////
//// Response
///////////////////////////////////////////////////////////////////////////////

HTTP::Response::Response() 
{
}

HTTP::Response::~Response() {

}

void 
HTTP::Response::clear()
{
	this->protocol.clear();
	int status = 0;
	this->reason.clear();
	this->headers.clear();
	this->data.reset();
}

void 
HTTP::Response::sendHeaders(Connection& conn) 
{
	std::stringstream responseData;
	responseData << protocol << " " << status << " " << reason << "\r\n";
	std::map<std::string,std::string>::iterator hdr;
	for (hdr = headers.begin(); hdr != headers.end(); ++hdr) {
		responseData << hdr->first << ": " << hdr->second << "\r\n";
	}
	responseData << "\r\n";

	conn.beginWrite(responseData);
}

void 
HTTP::Response::sendBody(Connection& conn) {
	conn.beginWrite(*this->data);
}


///////////////////////////////////////////////////////////////////////////////////////////
/// Cookie Free functions
///////////////////////////////////////////////////////////////////////////////////////////


// TODO: escape semicolons at least if not urlencode
void
HTTP::parseCookie(const std::string& cookieStr,CookieCollection& cookies)
{
	std::vector<std::string> items;
	Util::tokenize(cookieStr,items,"; ");
	std::pair<std::string,std::string> pair;
	for (std::vector<std::string>::iterator c = items.begin(); c != items.end(); c++) 
	{
		Util::parseNameValuePair(*c,'=',pair);
		cookies[pair.first] = pair.second;
	}
}