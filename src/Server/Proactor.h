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

	class SocketListener;
	class SocketConnection;
	class CompletionCtx;

	//////////////////////////////////////////////////////////////////////////
	/// Proactor 
	/// Provides an asynchronous IO interface
	class Proactor : public Singleton<Proactor>
	{
	public:

		/// Constructor
		Proactor();
		
		/// Destructor
		~Proactor();

		// Socket I/O
		void beginAccept(boost::shared_ptr<SocketListener> listener, 
						 EventHandler handler, 
						 boost::any tag = NULL);

		void beginRead(boost::shared_ptr<SocketConnection> conn, 
					   EventHandler handler, 
					   boost::any tag = NULL);
		
		void beginRead(boost::shared_ptr<SocketConnection> conn, 
					   EventHandler handler, 
					   boost::shared_ptr<IOMsgReadComplete> msgRead, 
					   boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<SocketConnection> conn, 
										  EventHandler handler, 
										  boost::shared_array<char> data, 
										  int len, 
										  boost::any tag = NULL);

		void beginWrite(boost::shared_ptr<SocketConnection> conn, 
						EventHandler handler, 
						boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
						boost::any tag = NULL);

		// Pipe I/O

		// Control
		void start(int numWorkers = 1);

		void stop(); 

	private:

		void runIO();

		bool doRecycle;
		bool doShutdown;

		std::vector<boost::thread *> workers;

		void completeSocketAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
		void completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
		void completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);

		#ifdef WIN32
			HANDLE iocp;
			LPFN_ACCEPTEX fnAcceptEx;
			LPFN_GETACCEPTEXSOCKADDRS fnGetAcceptExSockaddrs;
		#endif

	};

	inline void Proactor::beginRead(boost::shared_ptr<SocketConnection> conn, EventHandler handler, boost::any tag /* = NULL */)
	{
		boost::shared_ptr<IOMsgReadComplete> msgRead(new IOMsgReadComplete());
		this->beginRead(conn, handler, msgRead, tag);
	}

	inline void Proactor::beginWrite(boost::shared_ptr<SocketConnection> conn, 
									EventHandler handler, 
									boost::shared_array<char> data, 
									int len, 
									boost::any tag /* = NULL */)
	{
		boost::shared_ptr<IOMsgWriteComplete> msgWrite(new IOMsgWriteComplete());
		msgWrite->buf = data;
		msgWrite->len = len;
		this->beginWrite(conn, handler, msgWrite, tag);
	}

	class IOException : public StreamException<IOException> { };

#ifdef WIN32
	class CompletionCtx : public OVERLAPPED
	{
	public:

		boost::shared_ptr<IOMsg> msg;

		EventHandler handler;

		boost::any tag;
	};
#endif

}

#endif