#ifndef HTTP_H
#define HTTP_H

#include <time.h>
#include <map>
#include <sstream>

#include "IO/Connection.h"
#include "Utilities/Util.h"
#include "Utilities/Base64.h"
#include "Utilities/MTRand.h"
#include "IO/BufferChain.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/serialization/nvp.hpp"
#include "Session.h"

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
    class Controller;

    namespace HTTP
    {
		class Config;

        /// @class User
        /// @brief Object representing a web user
        class User
        {
        public:

            /// @brief Default Constructor
            User();

            /// @brief Constructor
            /// @param[in] username The username of the newly constructed User
            User(const std::string& username);

            /// @brief Get this User's username
            /// @retuns The User's username
            std::string getUsername() const;

            /// @brief Set this User's username
            /// @param[in] The username to set for this User
            void setUsername(std::string& username);

            /// @brief Get this User's password
            /// @retuns The User's password
            std::string getPassword() const;

            /// @brief Set this User's password
            /// @param[in] The password to set for this User
            void setPassword(const std::string& password);

            /// @brief Serialize this User to XML
            /// @param[in] ar Destination archive for the serialized data.
            /// @param[in] version The version of the archive. Used by boost::serialization version tracking.
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & BOOST_SERIALIZATION_NVP(username);
                ar & BOOST_SERIALIZATION_NVP(password);
            }

        private:

            std::string username;
            std::string password;

        };

        /// @brief Collection type for HTTP Users
        typedef std::map<std::string,User> UserCollection;	

        /// @brief Collection type for HTTP headers
        typedef std::map<std::string,std::string> HeaderCollection;

        /// @brief Collection type for HTTP querystring parameters
        typedef std::map<std::string,std::string> ParamCollection;

        /// @brief Collection type for HTTP Cookies
        typedef std::map<std::string,std::string> CookieCollection;	

        //////////////////////////////////////////////////////////////////////////        
        /// @class Request
        /// @brief An HTTP Request
        class Request 
        {
        public:

            /// @brief Constructor
            Request();

            /// @brief Destructor
            ~Request();

            /// @brief Reset this Request to an empty state, as if newly constructed
            void clear();

            /// @brief Asynchronously send the request headers over the specified Connection
            /// @param[in] conn The Connection on which to send the header data
            void sendHeaders(IO::Connection& conn);

            /// @brief Asynchronously send the request body over the specified Connection
            /// @param[in] conn The Connection on which to send the body data
            void sendBody(IO::Connection& conn);
            
            /// @brief Dump an HTML representation of the Request to the spcified output stream
            /// Useful for debugging
            void dump(std::ostream& info);

            /// @brief the URI portion of the request URL
            std::string requestURI;

            /// @brief the query string portion of the request URL
            std::string queryString;

            /// @brief The HTTP protocol and version specified in the request e.g. HTTP/1.1
            std::string protocol;

            /// @brief The HTTP request method e.g. GET, POST, HEAD, OPTIONS, PUT, DELETE
            std::string method;

            /// @brief The key-value parameter pairs passed to the request in the query string (int the case of a GET)
            /// or in the body (in the case of a POST)
            ParamCollection params;

            /// @brief The header name-value pairs in the request
            HeaderCollection headers;

            /// The cookies name-value pairs in the request
            CookieCollection cookies;

            /// A stream containing the request body
            boost::shared_ptr<std::iostream> data;

        };

        //////////////////////////////////////////////////////////////////////
        /// @class Request
        /// @brief An HTTP Request
        class Response 
        {
        public:

            /// @brief Constructor
            Response();

            /// @brief Destructor
            ~Response();

            /// @brief Clear the response object
            /// Resets this object to an empty state, suitable for reuse
            void clear();

            /// @brief Asynchronously send the response headers over the specified Connection
            /// @param[in] conn The Connection on which to send the header data
            void sendHeaders(IO::Connection& conn);

            /// @brief Asynchronously send the response body over the specified Connection
            /// @param[in] conn The Connection on which to send the body data
            void sendBody(IO::Connection& conn);

            /// @brief The protocol and version of the response e.g. HTTP/1.1
            std::string protocol;

            /// @The HTTP response status code e.g. 200, 401, 404
            int status;

            /// @The HTTP response status reason e.g. "OK", "Authorization Required", "Not Found"
            std::string reason;

            /// @brief The header name-value pairs in the response
            std::map<std::string,std::string> headers;

            /// A stream containing the response body
            boost::shared_ptr<std::iostream> data;
        };

        //////////////////////////////////////////////////////////////////////
        /// @class Env
        /// @brief The environment for an HTTPConnection
        /// This structure contains any HTTP-specific connection context
        class Env : public IO::ConnectionCtx
        {
        public:

            /// @brief Constructor
            Env() :
              req(NULL),
                  res(NULL),
                  controller(NULL),
                  session(NULL)
              {
              }

              /// @brief A pointer to the current Request object
              Request *req = nullptr;

              /// @brief A pointer to the current Response object
              Response *res = nullptr;

              /// @brief A shared pointer to the HTTP configuration
              boost::shared_ptr<HTTP::Config> cfg;

              /// @brief A pointer to the Controller for this connection
              /// This is the object that maps specific requests to "business logic"
              Controller *controller = nullptr;

              /// @brief A pointer to he HTTP Session for this connection
              Session *session = nullptr;

              /// A shared pointer to the Server which owns this Connection
              Server *server = nullptr;
        };


		class Config
		{
		public:

			HTTP::Config() :
				documentRoot(L""),
				addr("0.0.0.0"),
				port(5152),
				requireAuth(false),
				realm(L"")
			{}

			template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				ar & BOOST_SERIALIZATION_NVP(documentRoot);
				ar & BOOST_SERIALIZATION_NVP(addr);
				ar & BOOST_SERIALIZATION_NVP(port);
				ar & BOOST_SERIALIZATION_NVP(requireAuth);
				ar & BOOST_SERIALIZATION_NVP(users);
				ar & BOOST_SERIALIZATION_NVP(realm);
			}

			std::wstring documentRoot;
			std::string addr;
			unsigned short port;
			bool requireAuth;
			HTTP::UserCollection users;
			std::wstring realm;
		};


        //////////////////////////////////////////////////////////////////////
        /// Free functions
        //////////////////////////////////////////////////////////////////////

        /// @brief Parse a string containing cookies into a CookieCollection containing name-value pairs
        /// @param[in] cookieStr A string of raw cookie data
        /// @param[out] cookies A Collection of name-value pairs parsed from cookieStr
        void parseCookie(const std::string& cookieStr, CookieCollection& cookies);

        /// @brief Format the given time_t as an RFC1123 time string
        /// @param[in]timer A time_t object containing a time
        /// @returns A string containing the time in RFC1123 format e.g. Fri, 06 Mar 2009 19:10:14 GMT
        std::string getRFC1123(const time_t& timer);

        /// @brief Decode the given URL-encoded string
        /// @param[in,out] enc A URL-encoded string, which is decoded in-place
        void urlDecode(std::string& enc);

        /// @brief Decode the given URL-encoded string pair
        /// @param[in,out] pair A pair of URL-encoded strings, which are decoded in-place
        void urlDecode(std::pair<std::string,std::string>& pair);

        /// @brief URL-encode the given string
        /// @param[in] str a string to URL-encode
        /// @returns a URL-encoded version of str
        std::string urlEncode(const std::string& str);

        /// @brief Generate a nonce for digest authentication
        /// The nonce is of the form timestamp+MD5(timestamp+privatekey)
        /// @param[in] timestamp A time value used for generating the nonce
        /// @returns A string containing the generated nonce
        std::string genDigestNonce(time_t timestamp);

        /// @brief Generate a nonce for digest authentication using the current time as a timestamp
        /// The nonce is of the form timestamp+MD5(timestamp+privatekey)
        /// @returns A string containing the generated nonce
        std::string genDigestNonce();

        /// @brief Generate an opaque value for digest authentication
        /// @returns a 16-byte random hex string
        std::string genDigestOpaque();

        /// @brief Generate a valid digest response for comparing against the response sent by the client
        /// @param[in] authInfo map of name value pairs collected from the Authorization header in the request
        /// @param[in] method A string containing the auth method, e.g. "Digest" 
        /// @param[in] password The password of the user which is trying to authenticate
        /// @returns A valid HTTP digest response
        std::string genDigestResponse(std::map<std::string, std::string>& authInfo, const std::string& method, const std::string& password);

        /// @brief Try to authenticate a Request
        /// @param[in] env The HTTP environment containing the Request to authenticate
        /// @returns True if the Request contains valid credentials, false otherwise
        bool auth(const Env& env);

        /// @brief Attempt to parse a complete Request from the given bytes of data
        /// @param[in] data A BufferChain contining bytes of data to parse
        /// @param[out] req The Request object to populate from the data
        /// @param[out] length The number of bytes of data from the front of the chain used for the Request
        /// @returns true if a Request was successfully parsed, false otherwise. A false return value could
        /// simply indicate that more data is required to complete the Request.
        bool parseRequest(const IO::BufferChain<char>& data, Request& req, size_t& length);
    }	
}

#endif