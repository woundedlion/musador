#ifndef PROACTOR_A8167A71
#define PROACTOR_A8167A71

#include <vector>
#include "boost/signals.hpp"
#include "boost/any.hpp"
#include "boost/thread.hpp"
#include "Utilities/Singleton.h"
#include "Utilities/StreamException.h"
#include "Network/Network.h"
#include "IOMessages.h"
#include "Connection.h"

#ifdef WIN32
	#include <windows.h>
	#pragma comment(lib,"kernel32.lib")
	#include <mswsock.h>
	#pragma comment(lib,"mswsock.lib")
#endif

typedef boost::mutex Mutex;
typedef boost::mutex::scoped_lock Guard;

namespace Musador
{

	class Connection;
	class CompletionCtx;

	//////////////////////////////////////////////////////////////////////////
	/// Proactor 
	/// Provides an asynchronous IO interface
	class Proactor : public Singleton<Proactor>
	{
	public:

		typedef boost::function2<void, boost::shared_ptr<IOMsg>, boost::any> EventHandler;

		/// Constructor
		Proactor();
		
		/// Destructor
		~Proactor();

		void beginAccept(SOCKET listenSocket, 
						 boost::shared_ptr<ConnectionFactory> connFactory, 
						 EventHandler handler, 
						 boost::any tag = NULL);

		void beginRead(boost::shared_ptr<Connection> conn, 
					   EventHandler handler, 
					   boost::any tag = NULL);
		
		void beginRead(boost::shared_ptr<Connection> conn, 
					   EventHandler handler, 
					   boost::shared_ptr<IOMsgReadComplete> msgRead, 
					   boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<Connection> conn, 
										  EventHandler handler, 
										  boost::shared_array<char> data, 
										  int len, 
										  boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<Connection> conn, 
						EventHandler handler, 
						boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
						boost::any tag = NULL);


		void start(int numWorkers = 1);

		void stop(); 

	private:

		void runIO();

		bool doRecycle;
		bool doShutdown;

		std::vector<boost::thread *> workers;

		void completeAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
		void completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
		void completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);

		#ifdef WIN32
			HANDLE iocp;
			LPFN_ACCEPTEX fnAcceptEx;
			LPFN_GETACCEPTEXSOCKADDRS fnGetAcceptExSockaddrs;
		#endif

	};

	class IOException : public StreamException<IOException> { };

#ifdef WIN32
	class CompletionCtx : public OVERLAPPED
	{
	public:

		boost::shared_ptr<IOMsg> msg;

		Proactor::EventHandler handler;

		boost::any tag;
	};
#endif

}

#endif