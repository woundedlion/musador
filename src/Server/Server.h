#ifndef SERVER_A8167A71_4E20_466d_8D70_C211158BB00D
#define SERVER_A8167A71_4E20_466d_8D70_C211158BB00D

#include "Network/Network.h"
#include "Utilities/MIMEResolver.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include "IO/Connection.h"
#include "IO/Proactor.h"
#include "Session.h"
#include "Config/Config.h"

typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;
typedef boost::condition_variable Condition;
typedef boost::thread Thread;

namespace Musador
{
    /// @class Server
    /// @brief Generic Server class which can serve Connections for any type 
    /// of Listener
    class Server : public boost::enable_shared_from_this<Server>
    {
    public:

        /// @brief Constructor
        /// @param[in] cfg The configuration for this server
        Server(ServerConfig& cfg);

        /// @brief Destructor
        virtual ~Server();

        /// @brief Start accepting connections with the given listener
        /// @param[in] listener The listener for which to serve connections
        /// @param[in] tag User-defined data passed along to each new Connection's 
        /// onAcceptComplete() function as well as the Server's onAcceptComplete() function
        void acceptConnections(	boost::shared_ptr<IO::Listener> listener, 
            boost::any tag = NULL);

        /// @brief Asynchronously start the Server running
        void start();

        /// @brief Synchronously wait until the server is running
        void waitForStart();

        /// @brief Asynchronously shut down the Server
        void stop();

        /// @brief Synchronously wait until the server has shut down
        void waitForStop();

        /// @brief Asynchronously restart the server
        void restart();

        /// @brief Completion routine invoked when a new connection is accepted.
        /// @param[in] msg Shared pointer to a Msg object specifying the result of the asynchronous ACCEPT
        /// @param[in] tag User-defined data which was originally passed into Server::acceptConnections
        void onAcceptComplete(boost::shared_ptr<IO::Msg> msg, boost::any tag);

        /// @brief Invoked when on a Connection error
        /// @param[in] msg Shared pointer to the Connection on which the error occurred
        /// @param[in] msg The error which occurred
        void onError(boost::shared_ptr<IO::Connection> conn, const IO::Msg::ErrorCode& err);

        /// @brief Get the global Session collection for this Server
        Session & getSession(const std::string& key);

    private:

        void addConnection(boost::shared_ptr<IO::Connection> conn);
        void removeConnection(boost::shared_ptr<IO::Connection> conn);
        void killConnection(boost::shared_ptr<IO::Connection> conn);
        void killConnections();

        Musador::Network * net;

        ServerConfig& cfg;

        // Collection types
        typedef std::vector<boost::shared_ptr<IO::Connection> > ConnCollection;
        typedef std::vector<boost::shared_ptr<IO::Listener> > ListenerCollection;
        typedef std::map<std::string, boost::shared_ptr<Session> > SessionCollection;

        ConnCollection conns;
        Mutex connsMutex;

        volatile bool doRecycle;
        volatile bool doShutdown;

        ListenerCollection listeners;

        SessionCollection sessions;
        Mutex sessionsMutex;

        bool running;
        Mutex runningMutex;
        Condition runningCV;
    };
}

#endif
