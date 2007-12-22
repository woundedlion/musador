#ifndef SERVER_A8167A71_4E20_466d_8D70_C211158BB00D
#define SERVER_A8167A71_4E20_466d_8D70_C211158BB00D

#include "Network/Network.h"
#include "Utilities/MIMEResolver.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include "Protocol.h"
#include "Connection.h"
#include "Proactor.h"


typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;
typedef boost::condition Condition;
typedef boost::thread Thread;

namespace Musador
{
	using namespace Musador;

	class Server
	{
	public:

		class Config
		{
		public:
			Config() :
			  requireAuth(false),
				  filterRemoteIps(false),
				  allowIps(""),
				  denyIps(""),
				  maxConnections(100)
			  {
			  }

			  bool requireAuth;
			  bool filterRemoteIps;
			  const char * allowIps;
			  const char * denyIps;
			  std::string documentRoot;
			  int maxConnections;
			  std::string authRealm;
		};

		Server();

		Server(const Config& cfg);

		virtual ~Server();

		void start();

        void acceptConnections(	boost::shared_ptr<ProtocolFactory> protocolFactory, 
								const sockaddr_in& localEP, 
								int socketType = SOCK_STREAM, 
								int socketProto = IPPROTO_TCP);

		void waitForStart();

		void stop();

		void waitForStop();

		void restart();

		void runIO();

		void onAccept(boost::shared_ptr<IOMsg> msg, boost::any tag);

		void onRead(boost::shared_ptr<IOMsg> msg, boost::any tag);

		void onWrite(boost::shared_ptr<IOMsg> msg, boost::any tag);

		void onDisconnect(boost::shared_ptr<IOMsg> msg, boost::any tag);

		/*
		bool authReq(const Request& request, const StateStore& session);
		bool authIp(const std::string& ip);
		bool serveFile(const Request& request, const std::string& queryString, std::map<std::string,std::string> args, const Response& response);
		int getConnectionCount();
		virtual std::string getAuthRealm(const std::string& uri) = 0;
		virtual bool isValidUser(const std::string& username) = 0;
		StateStore * getSession(const std::string& key);
		*/

	private:

		Config cfg;
		Musador::Network * net;

		// State collections
		typedef std::map<SOCKET, boost::shared_ptr<Connection> > ConnCollection;
                typedef std::map<SOCKET,boost::shared_ptr<ProtocolFactory> > ListenerCollection;

		ConnCollection conns;
		Mutex connsMutex;

		// Start/Stop sync objects
		volatile bool doRecycle;
        volatile bool doShutdown;
		bool running;
		Mutex runningMutex;
        Condition runningCV;

		// Network
		ListenerCollection listenerProtocols;

		// I/O
		Proactor io;

		void addConnection(boost::shared_ptr<Connection> conn);
		void removeConnection(boost::shared_ptr<Connection> conn);
		void killConnection(boost::shared_ptr<Connection> conn);
		void killConnections();

	};
}

#endif