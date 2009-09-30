#ifndef PIPE_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define PIPE_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Listener.h"

namespace Musador
{
    namespace IO
    {
        class ConnectionCtx;

        /// @class PipeListener
        /// @brief Abstract base class for NamedPipe Listeners.
        /// Provides an asynchronous interface to listen for incoming NamedPipe connections.
        /// Specific protocol implementations, e.g. GUIListener are derived from this class 
        /// and implement the createConnection factory method to produce the correct PipeConnection type
        /// on a successful ACCEPT.
        class PipeListener : public Listener, public boost::enable_shared_from_this<PipeListener>
        {
        public:

            /// @brief Constructor.
            /// @param[in] name Unique name to identify this Listener's underlying NamedPipe.
            PipeListener(const std::wstring& name);

            /// @brief Destructor.
            ~PipeListener();

            /// @brief Begin an asynchronous ACCEPT with this Pipelistener.
            /// One incoming PipeConnection can be accepted for each outstanding invocation of this method.
            /// @param[in] handler EventHandler invoked when a Connection is accepted or the ACCEPT fails.
            /// @param[in] tag User-defined data passed along the handler upon completion of this request.
            void beginAccept(EventHandler handler, boost::any tag = NULL);

            /// @brief Get the unique name of this PipeListener's underlying NamedPipe.
            /// @returns The unique name identifying this Connection's underlying NamedPipe.
            std::wstring getName();

            /// @brief Get a string representation of this Listener.
            /// @returns A string representing this Listener.
            std::string toString();

        private:

            std::wstring name;

        };
    }
}

#endif