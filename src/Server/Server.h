#ifndef SERVER_A8167A71_4E20_466d_8D70_C211158BB00D
#define SERVER_A8167A71_4E20_466d_8D70_C211158BB00D

#include "Network/Network.h"
#include "Utilities/MIMEResolver.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include "ConnectionProcessor.h"
#include "Connection.h"
#include "Proactor.h"
#include "Session.h"
#include "Config/Config.h"


typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;
typedef boost::condition Condition;
typedef boost::thread Thread;

namespace Musador
{
	using namespace Musador;

	class Server : public ConnectionProcessor
	{
	public:

		Server(const ServerConfig& cfg);

		virtual ~Server();

		void start();

		template <class ConnType>
		void acceptConnections(	const sockaddr_in& localEP, 
								int socketType = SOCK_STREAM, 
								int socketProto = IPPROTO_TCP);

		void waitForStart();

		void stop();

		void waitForStop();

		void restart();

		void runIO();

		void onAccept(boost::shared_ptr<IOMsg> msg, boost::any tag);

		void onError(boost::shared_ptr<IOMsgError> msgErr);

		Session& getSession(const std::string& key);

		/*
		bool authReq(const Request& request, const StateStore& session);
		bool authIp(const std::string& ip);
		bool serveFile(const Request& request, const std::string& queryString, std::map<std::string,std::string> args, const Response& response);
		int getConnectionCount();
		virtual std::string getAuthRealm(const std::string& uri) = 0;
		virtual bool isValidUser(const std::string& username) = 0;
		*/

	private:

		Musador::Network * net;

		const ServerConfig& cfg;

		// Collection types
        typedef std::vector<boost::shared_ptr<Connection> > ConnCollection;
		typedef std::map<SOCKET,boost::shared_ptr<ConnectionFactory> > ListenerCollection;
		typedef std::map<std::string, boost::shared_ptr<Session> > SessionCollection;

		ConnCollection conns;
		Mutex connsMutex;

		volatile bool doRecycle;
        volatile bool doShutdown;
		bool running;
		Mutex runningMutex;
        Condition runningCV;
		boost::thread * ioThread;

		ListenerCollection listeners;
		SessionCollection sessions;

		void addConnection(boost::shared_ptr<Connection> conn);
		void removeConnection(boost::shared_ptr<Connection> conn);
		void killConnection(boost::shared_ptr<Connection> conn);
		void killConnections();

	};
}

#endif
