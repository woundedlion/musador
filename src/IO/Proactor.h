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
    namespace IO
    {
        class SocketListener;
        class SocketConnection;
        class PipeListener;
        class PipeConnection;
        class CompletionCtx;
        class Job;

        //////////////////////////////////////////////////////////////////////////
        /// Proactor 
        /// Provides an asynchronous IO interface
        class Proactor : public Util::Singleton<Proactor>
        {
        public:

            /// Constructor
            Proactor();

            /// Destructor
            ~Proactor();

            // Socket I/O
            void beginAccept(boost::shared_ptr<SocketListener> listener, EventHandler handler, boost::any tag = NULL);

            void beginRead(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::any tag = NULL);

            void beginRead(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgReadComplete> msgRead, 
                boost::any tag = NULL);

            void beginWrite(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::shared_array<char> data, 
                int len, 
                boost::any tag = NULL);

            void beginWrite(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgWriteComplete> msgWrite, 
                boost::any tag = NULL);

            // Pipe I/O
            void beginAccept(boost::shared_ptr<PipeListener> listener, 
                EventHandler handler, 
                boost::any tag = NULL);

            void beginConnect(boost::shared_ptr<PipeConnection> conn,
                EventHandler handler,
                const std::wstring& dest,
                boost::any tag = NULL);

            void beginRead(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::any tag = NULL);

            void beginRead(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgReadComplete> msgRead, 
                boost::any tag = NULL);

            void beginWrite(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::shared_array<char> data, 
                int len, 
                boost::any tag = NULL);

            void beginWrite(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgWriteComplete> msgWrite, 
                boost::any tag = NULL);

            // Control
            void start(int numWorkers = 1);

            void stop(); 

        private:

            typedef std::map<CompletionCtx *, boost::shared_ptr<CompletionCtx> > JobCollection;

            void runIO();

            void addJob(CompletionCtx * key, boost::shared_ptr<CompletionCtx> job);
            boost::shared_ptr<CompletionCtx> releaseJob(CompletionCtx * key);

            Mutex jobsMutex;
            JobCollection jobs;

            bool doRecycle;
            bool doShutdown;

            std::vector<boost::thread *> workers;

            void completeSocketAccept(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
            void completePipeAccept(boost::shared_ptr<CompletionCtx> ctx);
            void completeRead(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
            void completeWrite(boost::shared_ptr<CompletionCtx> ctx, unsigned long nBytes);
            void completeSocketConnect(boost::shared_ptr<CompletionCtx> ctx);
            void completePipeConnect(boost::shared_ptr<CompletionCtx> ctx);

#ifdef WIN32
            HANDLE iocp;
            LPFN_ACCEPTEX fnAcceptEx;
            LPFN_GETACCEPTEXSOCKADDRS fnGetAcceptExSockaddrs;
#endif

        };

        inline void Proactor::beginRead(boost::shared_ptr<SocketConnection> conn, EventHandler handler, boost::any tag /* = NULL */)
        {
            boost::shared_ptr<MsgReadComplete> msgRead(new MsgReadComplete());
            this->beginRead(conn, handler, msgRead, tag);
        }

        inline void Proactor::beginRead(boost::shared_ptr<PipeConnection> conn, EventHandler handler, boost::any tag /* = NULL */)
        {
            boost::shared_ptr<MsgReadComplete> msgRead(new MsgReadComplete());
            this->beginRead(conn, handler, msgRead, tag);
        }

        inline void Proactor::beginWrite(boost::shared_ptr<SocketConnection> conn, 
            EventHandler handler, 
            boost::shared_array<char> data, 
            int len, 
            boost::any tag /* = NULL */)
        {
            boost::shared_ptr<MsgWriteComplete> msgWrite(new MsgWriteComplete());
            msgWrite->buf = data;
            msgWrite->len = len;
            this->beginWrite(conn, handler, msgWrite, tag);
        }

        inline void Proactor::beginWrite(boost::shared_ptr<PipeConnection> conn, 
            EventHandler handler, 
            boost::shared_array<char> data, 
            int len, 
            boost::any tag /* = NULL */)
        {
            boost::shared_ptr<MsgWriteComplete> msgWrite(new MsgWriteComplete());
            msgWrite->buf = data;
            msgWrite->len = len;
            this->beginWrite(conn, handler, msgWrite, tag);
        }

        class IOException : public Util::StreamException<IOException> { };

#ifdef WIN32
        class CompletionCtx : public OVERLAPPED
        {
        public:

            boost::shared_ptr<Msg> msg;

            EventHandler handler;

            boost::any tag;
        };
#endif

    }
}

#endif