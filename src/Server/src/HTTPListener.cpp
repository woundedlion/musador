#include "HTTPListener.h"
#include "HTTPConnection.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;

HTTPListener::HTTPListener(const sockaddr_in& localEP) :
SocketListener(localEP,SOCK_STREAM,IPPROTO_TCP)
{}

boost::shared_ptr<Connection>
HTTPListener::createConnection()
{
    SOCKET s;
    try
    {
        s = Network::instance()->socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    catch (const NetworkException& e)
    {
        LOG(Error) << e.what();
        return boost::shared_ptr<Connection>();
    }

    boost::shared_ptr<HTTPConnection> conn(new HTTPConnection());
    conn->setSocket(s);
    return conn;
}