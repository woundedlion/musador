#ifndef HTTP_H
#define HTTP_H

#include <time.h>
#include <map>
#include <sstream>

#include "Connection.h"
#include "Utilities/Util.h"
#include "Utilities/MTRand.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "Session.h"
#include "Config/Config.h"

typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;

#define HTTP_HEADER_OK "HTTP/1.0 200 OK\r\n"
#define HTTP_HEADER_CONTENT "Content-Type: text/html\r\n"
#define HTTP_HEADER_TERM "\r\n"
#define HTTP_TERM "\r\n\r\n"
#define MAXREQSIZE 40960
#define HTTP_DIGEST_KEY "948c62d0eb714aff328da73d73e5d798"


namespace Musador
{

	namespace HTTP
	{

		//////////////////////////////////////////////////////////////////////
		/// User
		//////////////////////////////////////////////////////////////////////
		class User
		{
		public:

			User(const std::string& username);

			~User();

			void setPassword(const std::string& password);

			void authorize(const std::string& password);

		private:

			std::string username;
			std::string password;

		};

		//////////////////////////////////////////////////////////////////////
		/// CookieStore
		//////////////////////////////////////////////////////////////////////
		class CookieStore : public Session
		{
		public:
			CookieStore(const std::string& cookieStr);
			std::string toString();
		};

		typedef std::map<std::string,std::string> HeaderCollection;
		typedef std::map<std::string,std::string> ParamCollection;

		//////////////////////////////////////////////////////////////////////
		/// Request
		//////////////////////////////////////////////////////////////////////
		class Request 
		{
		public:

			Request();
			~Request();

			void clear();
			void sendHeaders(Connection& conn);
			void sendBody(Connection& conn);

			std::string requestURI;
			std::string queryString;
			std::string protocol;
			std::string method;
			ParamCollection params;
			HeaderCollection headers;
			std::string authString;
			std::stringstream data;

			void requestInfo(std::stringstream& info);

		private:

		};

		//////////////////////////////////////////////////////////////////////
		/// Response
		//////////////////////////////////////////////////////////////////////
		class Response 
		{
		public:
			Response();
			~Response();

			void clear();
			void sendHeaders(Connection& conn);
			void sendBody(Connection& conn);

			std::string protocol;
			int status;
			std::string reason;
			std::map<std::string,std::string> headers;
			std::stringstream data;

		};

		//////////////////////////////////////////////////////////////////////
		/// Env
		//////////////////////////////////////////////////////////////////////
		class Env : public ConnectionCtx
		{
		public:

			Env() :
			req(NULL),
			res(NULL),
			cfg(NULL)
			{
			}
			Request * req;
			Response * res;
			const SiteConfig * cfg;
		};

		//////////////////////////////////////////////////////////////////////
		/// Free functions
		//////////////////////////////////////////////////////////////////////

		std::string getRFC1123(const time_t& timer);

		void urlDecode(std::string& enc);

		void urlDecode(std::pair<std::string,std::string>& pair);

		std::string urlEncode(const std::string& enc);

		void genDigestNonce(std::string& nonce, time_t timestamp);

		void genDigestNonce(std::string& nonce);

		void genDigestOpaque(std::string& opaque);

		void genDigestResponse(std::string& response, std::map<std::string, std::string>& authInfo, const std::string& method, const std::string& password);

	}	

}

#endif