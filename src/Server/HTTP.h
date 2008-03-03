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

	
		typedef std::map<std::string,std::string> HeaderCollection;
		typedef std::map<std::string,std::string> ParamCollection;
		typedef std::map<std::string,std::string> CookieCollection;	

		//////////////////////////////////////////////////////////////////////
		/// Cookie free functions
		//////////////////////////////////////////////////////////////////////
		void parseCookie(const std::string& cookieStr, CookieCollection& cookies);

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
			void dump(std::ostream& info);

			std::string requestURI;
			std::string queryString;
			std::string protocol;
			std::string method;
			ParamCollection params;
			HeaderCollection headers;
			CookieCollection cookies;
			boost::shared_ptr<std::iostream> data;

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
			boost::shared_ptr<std::iostream> data;
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
			cfg(NULL),
			controller(NULL),
			session(NULL)
			{
			}

			Request * req;
			Response * res;
			const SiteConfig * cfg;
			Controller * controller;
			Session * session;
		};

		//////////////////////////////////////////////////////////////////////
		/// Free functions
		//////////////////////////////////////////////////////////////////////

		std::string getRFC1123(const time_t& timer);

		void urlDecode(std::string& enc);

		void urlDecode(std::pair<std::string,std::string>& pair);

		std::string urlEncode(const std::string& enc);

		std::string genDigestNonce(time_t timestamp);

		std::string genDigestNonce();

		std::string genDigestOpaque();

		std::string genDigestResponse(std::map<std::string, std::string>& authInfo, const std::string& method, const std::string& password);

		bool auth(const Env& env);
	}	

}

#endif