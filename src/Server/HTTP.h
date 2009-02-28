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
    class HTTPConfig;
    class Controller;

    namespace HTTP
    {

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

            /// @brief Send the request headers over the specified Connection
            /// @param[in] conn The IO:Connection object on which the header data is sent
            void sendHeaders(IO::Connection& conn);

            /// @brief Send the Request body over the specified Connection
            /// @param[in] conn The IO:Connection object on which the request data is sent
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

            /// A stream containing the request data (body of the request)
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
            void sendHeaders(IO::Connection& conn);
            void sendBody(IO::Connection& conn);

            std::string protocol;
            int status;
            std::string reason;
            std::map<std::string,std::string> headers;
            boost::shared_ptr<std::iostream> data;
        };

        //////////////////////////////////////////////////////////////////////
        /// Env
        //////////////////////////////////////////////////////////////////////
        class Env : public IO::ConnectionCtx
        {
        public:

            Env() :
              req(NULL),
                  res(NULL),
                  controller(NULL),
                  session(NULL)
              {
              }

              Request * req;
              Response * res;
              boost::shared_ptr<HTTPConfig> cfg;
              Controller * controller;
              Session * session;
              boost::shared_ptr<Server> server;
        };

        //////////////////////////////////////////////////////////////////////
        /// Free functions
        //////////////////////////////////////////////////////////////////////

        void parseCookie(const std::string& cookieStr, CookieCollection& cookies);

        std::string getRFC1123(const time_t& timer);

        void urlDecode(std::string& enc);

        void urlDecode(std::pair<std::string,std::string>& pair);

        std::string urlEncode(const std::string& enc);

        std::string genDigestNonce(time_t timestamp);

        std::string genDigestNonce();

        std::string genDigestOpaque();

        std::string genDigestResponse(std::map<std::string, std::string>& authInfo, const std::string& method, const std::string& password);

        bool auth(const Env& env);

        bool parseRequest(const IO::BufferChain<char>& data, Request& req, size_t& length);
    }	

}

#endif