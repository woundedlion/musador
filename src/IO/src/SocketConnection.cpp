#include "boost/bind.hpp"
#include "SocketConnection.h"
#include "Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"
using namespace Musador;
using namespace Musador::IO;

SocketConnection::SocketConnection() :
sock(NULL)
{

}

SocketConnection::~SocketConnection()
{
    this->close();
}

void 
SocketConnection::beginRead(boost::any tag /* = NULL */)
{
    Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onReadComplete,this,_1,_2));
}

void 
SocketConnection::beginRead(boost::shared_ptr<MsgReadComplete> msgRead, boost::any tag /* = NULL */)
{
    Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onReadComplete,this,_1,_2), msgRead);
}

inline
void 
SocketConnection::beginWrite(boost::shared_ptr<MsgWriteComplete> msgWrite, 
                             boost::any tag /* = NULL */)
{
    Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), msgWrite, tag);
}


void 
SocketConnection::beginWrite(const Buffer<char>& data, boost::any tag /* = NULL */)
{
    Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, tag);
}

void 
SocketConnection::beginWrite(std::istream& dataStream, boost::any tag /* = NULL */)
{
    boost::shared_array<char> data(new char[MsgWriteComplete::MAX]);
    dataStream.read(data.get(),MsgWriteComplete::MAX);
    size_t len = static_cast<size_t>(dataStream.gcount());
    Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), Buffer<char>(data, len, len), tag);
}

void 
SocketConnection::beginWrite(const std::string& str, boost::any tag /* = NULL */)
{
    size_t len = str.size();
    boost::shared_array<char> data(new char[len]);
    str.copy(data.get(), len);
    Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), Buffer<char>(data, len, len), tag);
}

void 
SocketConnection::close()
{
    if (NULL != this->sock)
    {
        try
        {
            LOG(Debug) << " Closing listening socket " << this->sock;
            Network::instance()->closeSocket(this->sock);
            this->sock = NULL;
        }
        catch(const NetworkException& e)
        {
            LOG(Warning) << e.what() << " while closing client socket.";
        }
    }
}

SOCKET 
SocketConnection::getSocket()
{
    return this->sock;
}

void 
SocketConnection::setSocket(SOCKET s)
{
    this->sock = s;
}

sockaddr_in 
SocketConnection::getLocalEP()
{
    return this->localEP;
}

void 
SocketConnection::setLocalEP(sockaddr_in localEP)
{
    this->localEP = localEP;
}

sockaddr_in 
SocketConnection::getRemoteEP()
{
    return this->remoteEP;
}

void 
SocketConnection::setRemoteEP(sockaddr_in remoteEP)
{
    this->remoteEP = remoteEP;
}

std::string 
SocketConnection::toString()
{
    std::stringstream r;
    r << ::inet_ntoa(this->remoteEP.sin_addr) << ":" << ::ntohs(this->remoteEP.sin_port) << " => " 
        << ::inet_ntoa(this->localEP.sin_addr) << ":" << ::ntohs(this->localEP.sin_port)
        << " [" << this->getSocket() << "]";
    return r.str();
}

