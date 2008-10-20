#ifndef CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D
#define CONNECTION_A8167A71_4E20_466d_8D70_C211158BB00D

#include <sstream>
#include "boost/function.hpp"
#include <boost/shared_ptr.hpp>
#include "boost/any.hpp"
#include "Network/Network.h"
#include "Utilities/Factory.h"
#include "IOMessages.h"

namespace Musador
{
    class Server;

    namespace IO
    {
        typedef AbstractFactory<Connection> ConnectionFactory;

        class ConnectionCtx
        {
        };

        class Connection
        {
        public:

            virtual ~Connection() {};

            virtual void beginConnect(boost::any tag = NULL) = 0;

            virtual void beginRead(boost::any tag = NULL) = 0;
            virtual void beginRead(boost::shared_ptr<MsgReadComplete> msgRead, 
                boost::any tag = NULL) = 0;

            virtual void beginWrite(boost::shared_ptr<MsgWriteComplete> msgWrite, 
                boost::any tag = NULL) = 0;
            virtual void beginWrite(boost::shared_array<char> data, unsigned int len, boost::any tag = NULL) = 0;
            virtual void beginWrite(std::istream& dataStream, boost::any tag = NULL) = 0;
            virtual void beginWrite(const std::string& str, boost::any tag = NULL) = 0;

            virtual void close() = 0;

            virtual std::string toString() = 0;

            virtual void onAcceptComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;
            virtual void onConnectComplete(boost::shared_ptr<Msg>, boost::any tag = NULL) = 0;		
            virtual void onReadComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;
            virtual void onWriteComplete(boost::shared_ptr<Msg> msg, boost::any tag = NULL) = 0;

        };
    }
}

#endif
