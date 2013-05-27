#ifndef PROACTOR_A8167A71
#define PROACTOR_A8167A71

#include <vector>
#include <boost/signals.hpp>
#include <boost/any.hpp>
#include <boost/thread.hpp>
#include "Utilities/Singleton.h"
#include "Utilities/StreamException.h"
#include "Network/Network.h"
#include "IOMessages.h"
#include "Connection.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
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

        /// @class Proactor 
        /// @brief Implementation of the Proactor Pattern providing an interface to perform asynchronous I/O operations.
        class Proactor : public Util::Singleton<Proactor>
        {
        public:

            /// @brief Constructor.
            Proactor();

            /// @brief Destructor
            ~Proactor();

            // Socket I/O

            /// @brief Asynchronously accept a connection on a Socket.
            /// @param[in] listener Shared pointer to the SocketListener on which to accept a connection.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginAccept(boost::shared_ptr<SocketListener> listener, EventHandler handler, boost::any tag = NULL);

            /// @brief Asynchronously read from a Socket.
            /// @param[in] conn Shared pointer to the SocketConnection from which to read.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginRead(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::any tag = NULL);

            /// @brief Asynchronously read from a Socket using a previously allocated completion message.
            /// @param[in] conn Shared pointer to the SocketConnection from which to read.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] msgRead A MsgReadComplete used to store the data read from the connection.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginRead(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgReadComplete> msgRead, 
                boost::any tag = NULL);

            /// @brief Asynchronously write to a Socket using a previously allocated array.
            /// @param[in] conn Shared pointer to the SocketConnection to which to write.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] data A Buffer containing data to write to the connection.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginWrite(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                const Buffer<char>& data, 
                boost::any tag = NULL);

            /// @brief Asynchronously write to a Socket using a previously allocated completion message.
            /// @param[in] conn Shared pointer to the SocketConnection to which to write.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] msgWrite Shared pointer to a MsgWriteComplete containing the data to write to the connection.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginWrite(boost::shared_ptr<SocketConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgWriteComplete> msgWrite, 
                boost::any tag = NULL);

            // Pipe I/O

            /// @brief Asynchronously accept a connection on a NamedPipe.
            /// @param[in] listener Shared pointer to the PipeListener on which to accept a connection.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginAccept(boost::shared_ptr<PipeListener> listener, 
                EventHandler handler, 
                boost::any tag = NULL);

            /// @brief Asynchronously connect to a NamedPipe.
            /// @param[in] conn Shared pointer to the PipeConnection with which to connect.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] dest The name of the pipe to connect to
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginConnect(boost::shared_ptr<PipeConnection> conn,
                EventHandler handler,
                const std::wstring& dest,
                boost::any tag = NULL);

            /// @brief Asynchronously read from a Named Pipe.
            /// @param[in] conn Shared pointer to the PipeConnection from which to read.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginRead(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::any tag = NULL);

            /// @brief Asynchronously read from a Named Pipe using a previously allocated completion message.
            /// @param[in] conn Shared pointer to the PipeConnection from which to read.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] msgRead A MsgReadComplete used to store the data read from the connection.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginRead(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgReadComplete> msgRead, 
                boost::any tag = NULL);

            /// @brief Asynchronously write to a Named Pipe using a previously allocated array.
            /// @param[in] conn Shared pointer to the PipeConnection to which to write.
            /// @param[in] handler The EventHandler which is called back on completion or error.
            /// @param[in] data A Buffer containing data to write to the connection.
            /// @param[in] tag User-defined data which are passed along to handler on completion or error.
            void beginWrite(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                const Buffer<char>& data, 
                boost::any tag = NULL);

           /// @brief Asynchronously write to a Named Pipe using a previously allocated completion message.
           /// @param[in] conn Shared pointer to the PipeConnection to which to write.
           /// @param[in] handler The EventHandler which is called back on completion or error.
           /// @param[in] msgWrite Shared pointer to a MsgWriteComplete containing the data to write to the connection.
           /// @param[in] tag User-defined data which are passed along to handler on completion or error.
           void beginWrite(boost::shared_ptr<PipeConnection> conn, 
                EventHandler handler, 
                boost::shared_ptr<MsgWriteComplete> msgWrite, 
                boost::any tag = NULL);

           /// non-I/O Events

           /// @brief Asynchronously invoke a function
           /// @param[in] f The function to invoke asynronously from an IO thread
           void beginInvoke(boost::function<void ()> f);

           // Control Methods

           /// @brief Start the IO engine running.
           /// A user must call this before I/O requests will be serviced.
           /// @param[in] numWorkers The number of worker threads to spawn which will service I/O requests.
           /// @remarks The Proactor will attempt to maintain one active worker thread for each core
           /// available on the machine. So, for instance, a good value for numWorkers would be double the number of cores.
           /// That way, if any I/O threads are sleeping, the Proactor will automatically wake additional threads from the worker pool 
           /// to service outstanding I/O requests, to fill out the number of currently active threads until it again
           /// reaches the # of available cores. A value of 0, which is the default, will automatically use a value of double 
           /// the hardware concurrency available on the machine.
           void start(int numWorkers = 0);

           /// @brief Stop the I/O engine. A user must call this to shut down the Proactor and its worker threads.
           void stop(); 

        private:

#ifdef WIN32
            class Job : public OVERLAPPED
#else
            class Job
#endif
            {
                friend class Proactor;

            public:

                Job();
				~Job () {}

            private:

                boost::shared_ptr<Msg> msg;
                EventHandler handler;
                boost::any tag;
            };

            void runIO();

			std::unique_ptr<Job> makeJob();
			void postJob(std::unique_ptr<Job> job);

            void completeSocketAccept(std::unique_ptr<Job> ctx, unsigned long nBytes);
            void completePipeAccept(std::unique_ptr<Job> ctx);
            void completeRead(std::unique_ptr<Job> ctx, unsigned long nBytes);
            void completeWrite(std::unique_ptr<Job> ctx, unsigned long nBytes);
            void completeSocketConnect(std::unique_ptr<Job> ctx);
            void completePipeConnect(std::unique_ptr<Job> ctx);
            void completeInvoke(std::unique_ptr<Job> ctx);

            bool doRecycle;
            bool doShutdown;

            std::vector<boost::thread *> workers;

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
            const Buffer<char>& data, 
            boost::any tag /* = NULL */)
        {
            boost::shared_ptr<MsgWriteComplete> msgWrite(new MsgWriteComplete(data));
            this->beginWrite(conn, handler, msgWrite, tag);
        }

        inline void Proactor::beginWrite(boost::shared_ptr<PipeConnection> conn, 
            EventHandler handler, 
            const Buffer<char>& data, 
            boost::any tag /* = NULL */)
        {
            boost::shared_ptr<MsgWriteComplete> msgWrite(new MsgWriteComplete(data));
            this->beginWrite(conn, handler, msgWrite, tag);
        }

        class IOException : public Util::StreamException<IOException> { };
    }
}

#endif