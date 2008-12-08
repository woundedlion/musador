#ifndef PIPE_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define PIPE_CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"

namespace Musador
{
    namespace IO
    {
        /// @class PipeConnection
        /// @brief Abstract base class for NamedPipe connections.
        /// Provides an asynchronous interface for performing I/O on NamedPipe connections.
        /// Specific protocol implementations, e.g. GUIConnection
        /// are derived from this class and implement the on* methods to provide protocol logic.
        class PipeConnection : public Connection, public boost::enable_shared_from_this<PipeConnection>
        {
        public:

            /// @brief Constructor.
            /// @param[in] name Unique name to identify this Connection's underlying NamedPipe.
            PipeConnection(const std::wstring& name);

            /// @brief Destructor.
            ~PipeConnection();

            /// @brief Start an asynchronous CONNECT.
            /// Upon completion, the IO engine will invoke the onConnectComplete method.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onConnectComplete.
            void beginConnect(boost::any tag = NULL);

            /// @brief Start an asynchronous READ.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onReadComplete.
            void beginRead(boost::any tag = NULL);
            
            /// @brief Start an asynchronous READ using a previously allocated read completion message.
            /// Upon completion, the IO engine will invoke the onReadComplete method.
            /// @param[in] msgRead Msg object used to store any data read from the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onReadComplete.
            void beginRead(boost::shared_ptr<MsgReadComplete> msgRead, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using a previously allocated write completion message.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] msgWrite Msg object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(boost::shared_ptr<MsgWriteComplete> msgWrite, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using a previously allocated array.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] data Buffer object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(const Buffer<char>& data, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using data from an input stream.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] dataStream std::istream object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(std::istream& dataStream, boost::any tag = NULL);

            /// @brief Start an asynchronous WRITE using data from a std::string.
            /// Upon completion, the IO engine will invoke the onWriteComplete method.
            /// @param[in] str std::string object containing data to write to the connection.
            /// @param[in] tag User-defined data which, upon completion, are passed along to onWriteComplete.
            void beginWrite(const std::string& str, boost::any tag = NULL);

            /// @brief Close the client end if the NamedPipe.
            void close();

            /// @brief Get a string representation of this PipeConnection.
            /// @returns A UTF-8 encoding of the unique name identifying this Connection's underlying NamedPipe.
            std::string toString();

            /// @brief Get the unique name of this PipeConnection's underlying NamedPipe.
            /// @returns The unique name identifying this Connection's underlying NamedPipe.
            std::wstring getName();

            /// @brief Get this PipeConnection's underlying NamedPipe instance.
            /// @returns a system-specific handle to this PipeConnection's underlying NamedPipe instance.
            HANDLE getPipe();

            /// @brief Set the underlying NamedPipe instance used by this PipeConnection.
            /// @param[in] pipe A system-specific handle to a NamedPipe instance to be used for this PipeConnection.
            void setPipe(HANDLE pipe);

        private:

            std::wstring name;
            HANDLE pipe;

        };
    }
}

#endif