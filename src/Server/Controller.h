#ifndef CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290
#define CONTROLLER_H_2B3977A0_7D74_4785_B61B_EECC28F9D290

#include <map>
#include "boost/function.hpp"

namespace Musador
{
    namespace HTTP
    {
        /// @class Env
        /// @brief Represents the environment for a particular HTTP connection
        class Env;
    }

    /// @class Controller
    /// @brief As in MVC, implements logic for processing and responding to HTTP Requests
    class Controller
    {
    public:

        /// @brief Function object type for functions which handle a particular Request
        typedef boost::function1<bool, HTTP::Env &> Handler;

        /// @brief Destructor
        virtual ~Controller() {}

        /// @brief Dispatches the Request tot he appropriate handler, if any
        /// @param[in] env The Environment containing the HTTP Request to dispatch
        /// @returns true if a handler was found for the request, false otherwise
        virtual bool exec(HTTP::Env& env);	

        /// @brief Register a handler for the given Request URI
        /// @param[in] requestURI The URI to map to the given Handler
        /// @param[in] handler A function object for handling the given Request URI
        void addHandler(const std::string& requestURI, Handler handler);

    private:

        typedef std::map<std::string,Handler> HandlerMap;

        HandlerMap handlers;

    };

}

#endif