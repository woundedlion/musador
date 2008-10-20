#ifndef IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D
#define IO_MSGS_A8167A71_4E20_466d_8D70_C211158BB00D

#include <assert.h>
#include <boost/function.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include "Network/Network.h"

namespace Musador
{
    namespace IO
    {
        class Connection;
        class Listener;

        enum MsgType
        {
            MSG_SOCKET_ACCEPT_COMPLETE,
            MSG_PIPE_ACCEPT_COMPLETE,
            MSG_WRITE_COMPLETE,
            MSG_READ_COMPLETE,
            MSG_SOCKET_CONNECT_COMPLETE,
            MSG_PIPE_CONNECT_COMPLETE,
            MSG_ERROR,
            MSG_SHUTDOWN
        };

        class Msg
        {
        public:

            Msg(MsgType type) : type(type) {}

            inline MsgType getType() { return type; }

            boost::shared_ptr<Connection> conn;

        private: 

            MsgType type;
        };

        class MsgShutdown : public Msg
        {
        public:
            inline MsgShutdown() : Msg(MSG_SHUTDOWN) {}
        };

        class MsgWriteComplete : public Msg
        {
        public:

            inline MsgWriteComplete() : Msg(MSG_WRITE_COMPLETE),
                buf(new char[MsgWriteComplete::MAX]),
                len(0),
                off(0)
            {
            }

            boost::shared_array<char> buf;
            unsigned long len;
            unsigned long off;
            static const int MAX = 4096;
        };

        class MsgReadComplete : public Msg
        {
        public:

            inline MsgReadComplete() : Msg(MSG_READ_COMPLETE),
                buf(new char[MsgReadComplete::MAX]),
                len(0),
                off(0)
            {
            }

            boost::shared_array<char> buf;
            unsigned long len;
            unsigned long off;
            static const int MAX = 4096;
        };

        class MsgSocketAcceptComplete : public Msg
        {
        public:

            inline MsgSocketAcceptComplete() : Msg(MSG_SOCKET_ACCEPT_COMPLETE),
                buf(new char[2 * (sizeof(sockaddr_in) + 16)]),
                len(0),
                off(0)
            {
            }

            boost::shared_ptr<Listener> listener;
            boost::shared_array<char> buf;
            unsigned long len;
            unsigned long off;
        };

        class MsgPipeAcceptComplete : public Msg
        {
        public:

            inline MsgPipeAcceptComplete() : Msg(MSG_PIPE_ACCEPT_COMPLETE)
            {
            }

            boost::shared_ptr<Listener> listener;
        };

        class MsgSocketConnectComplete : public Msg
        {
        public:

            inline MsgSocketConnectComplete() : Msg(MSG_SOCKET_CONNECT_COMPLETE)
            {
            }
        };

        class MsgPipeConnectComplete : public Msg
        {
        public:

            inline MsgPipeConnectComplete() : Msg(MSG_PIPE_CONNECT_COMPLETE)
            {
            }
        };

        class MsgError : public Msg
        {
        public:

            inline MsgError() : Msg(MSG_ERROR),
                err(0)
            {
            }

            int err;
            boost::any tag;

        };

        typedef boost::function2<void, boost::shared_ptr<Msg>, boost::any> EventHandler;

    }
}

#include "Connection.h"

#endif