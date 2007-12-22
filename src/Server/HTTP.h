#ifndef HTTP_H
#define HTTP_H

#include <time.h>
#include <map>
#include <sstream>

#include "Utilities/Util.h"
#include "Utilities/MTRand.h"
#include "Codec.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread\mutex.hpp"
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
		/// StateStore
		//////////////////////////////////////////////////////////////////////
		class StateStore 
		{
		public:
			StateStore();
			virtual ~StateStore();
			std::string& operator[](const std::string& key);
			void clear();
		protected:
			Mutex lock;
			std::map<std::string,std::string> store;
		};

		//////////////////////////////////////////////////////////////////////
		/// CookieStore
		//////////////////////////////////////////////////////////////////////
		class CookieStore : public StateStore 
		{
		public:
			CookieStore(const std::string& cookieStr);
			std::string toString();
		};

		//////////////////////////////////////////////////////////////////////
		/// Request
		//////////////////////////////////////////////////////////////////////
		class Request 
		{
		public:
			enum reqMeths { GET, HEAD, POST, PUT };

			Request();
			~Request();

			void receiveFrom(SOCKET remoteSocket);
			int sendHeaders();
			int sendRaw(const char * data,int size);
			void setData(const std::string&);
			int sendRequest();

			SOCKET remoteSocket;

			std::string request;
			std::string requestURI;
			std::string queryString;
			std::string protocol;
			int status;
			std::string reason;
			reqMeths method;	
			std::map<std::string,std::string> params;
			std::map<std::string,std::string> headers;
			std::string data;
			std::string authString;

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

			void receiveFrom(SOCKET remoteSocket);
			int sendHeaders();
			int sendRaw(const char * data,unsigned int size);
			void setData(const std::string&);
			const std::string& getData();
			int sendResponse();

			SOCKET remoteSocket;
			std::string protocol;
			int status;
			std::string reason;
			std::map<std::string,std::string> headers;
		private:
			std::string data;
		};

		//////////////////////////////////////////////////////////////////////
		/// Env
		//////////////////////////////////////////////////////////////////////
		struct Env 
		{
			boost::shared_ptr<Request> request;
			boost::shared_ptr<Response> response;
			boost::shared_ptr<StateStore> session;
			boost::shared_ptr<CookieStore> cookies;
			std::map<std::string,std::string> args;
			std::string sessionName;
		};

		//////////////////////////////////////////////////////////////////////
		/// Free functions
		//////////////////////////////////////////////////////////////////////

		std::string * getRFC1123(time_t timer,std::string * time);

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