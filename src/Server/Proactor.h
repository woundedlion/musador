#ifndef PROACTOR_A8167A71
#define PROACTOR_A8167A71

#include <vector>
#include "boost/signals.hpp"
#include "boost/any.hpp"
#include "boost/thread.hpp"
#include "Utilities/Singleton.h"
#include "Network/Network.h"
#include "IOMessages.h"

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

		void runIO();

		void beginAccept(SOCKET listenSocket, EventHandler handler, boost::any tag = NULL);
		void beginAccept(SOCKET listenSocket, EventHandler handler, boost::shared_ptr<IOMsgAcceptComplete> msgAccept, boost::any tag = NULL);

		void beginRead(boost::shared_ptr<Connection> conn, EventHandler handler, boost::any tag = NULL);
		void beginRead(boost::shared_ptr<Connection> conn, EventHandler handler, boost::shared_ptr<IOMsgReadComplete> msgRead, boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<Connection> conn, EventHandler handler, boost::shared_array<char> data, int len, boost::any tag = NULL);
		void beginWrite(boost::shared_ptr<Connection> conn, EventHandler handler, boost::shared_ptr<IOMsgWriteComplete> msgWrite, boost::any tag = NULL);

		volatile bool doRecycle;
		volatile bool doShutdown;

	private:

		void completeAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
		void completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
		void completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);

		#ifdef WIN32
			HANDLE iocp;
			LPFN_ACCEPTEX fnAcceptEx;
			LPFN_GETACCEPTEXSOCKADDRS fnGetAcceptExSockaddrs;
		#endif

	};

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



#include "Connection.h"

#endif