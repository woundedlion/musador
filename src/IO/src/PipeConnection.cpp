#include "boost/bind.hpp"
#include "PipeConnection.h"
#include "Utilities/Util.h"
#include "Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"
using namespace Musador;

PipeConnection::PipeConnection(const std::wstring& name) :
name(name),
pipe(NULL)
{
}

PipeConnection::~PipeConnection()
{
	this->close();
}

void 
PipeConnection::close()
{
	if (NULL != this->pipe)
	{
		::DisconnectNamedPipe(this->pipe);
		::CloseHandle(this->pipe);
		this->pipe = NULL;
	}
}

void
PipeConnection::beginConnect(boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginConnect(this->shared_from_this(), boost::bind(&Connection::onConnectComplete,this,_1,_2), this->getName(), tag);
}

void 
PipeConnection::beginRead(boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onReadComplete,this,_1,_2), tag);
}

void 
PipeConnection::beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead, boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onReadComplete,this,_1,_2), msgRead, tag);
}

void 
PipeConnection::beginWrite(boost::shared_ptr<IOMsgWriteComplete> msgWrite, boost::any tag /* = NULL */)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), msgWrite, tag);
}

void 
PipeConnection::beginWrite(boost::shared_array<char> data, unsigned int len, boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, len, tag);
}

void 
PipeConnection::beginWrite(std::istream& dataStream, boost::any tag /*= NULL*/)
{
	boost::shared_array<char> data(new char[IOMsgWriteComplete::MAX]);
	dataStream.read(data.get(),IOMsgWriteComplete::MAX);
	int len = dataStream.gcount();
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, len, tag);
}

void 
PipeConnection::beginWrite(const std::string& str, boost::any tag /*= NULL*/)
{
	unsigned int len = str.size();
	boost::shared_array<char> data(new char[len]);
	str.copy(data.get(), len);
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, len, tag);
}

std::string 
PipeConnection::toString()
{
	return Util::unicodeToUtf8(this->name);
}

std::wstring
PipeConnection::getName()
{
	return this->name;
}

HANDLE 
PipeConnection::getPipe()
{
	return this->pipe;
}

void 
PipeConnection::setPipe(HANDLE pipe)
{
	this->pipe = pipe;
}

std::string
PipeConnection::friendlyName()
{
	std::string r = Util::unicodeToUtf8(this->name);
	size_t pos = r.find_last_of("\\/");
	if (pos == std::string::npos)
	{
		return r;
	} 
	else 
	{
		return r.substr(r.find_last_of("\\/") + 1);
	}
}