#include "EchoListener.h"
#include "EchoConnection.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;

EchoListener::EchoListener(const sockaddr_in& localEP) :
SocketListener(localEP,SOCK_STREAM,IPPROTO_TCP)
{}

boost::shared_ptr<IO::Connection>
EchoListener::createConnection()
{
    SOCKET s;
    try
    {
        s = Network::instance()->socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    catch (const NetworkException& e)
    {
        LOG(Error) << e.what();
        return boost::shared_ptr<IO::Connection>();
    }

    boost::shared_ptr<EchoConnection> conn(new EchoConnection());
    conn->setSocket(s);
    return conn;
}