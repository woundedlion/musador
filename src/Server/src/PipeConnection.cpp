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
    HANDLE p  = ::CreateFile(name.c_str(),
                             FILE_READ_DATA | FILE_WRITE_DATA,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_OVERLAPPED,
                             NULL
                             );

    if (INVALID_HANDLE_VALUE == p)
    {
        LOG(Error) << "CreateFile() failed to open name pipe: " << name << " [" << ::GetLastError() << "]";
        return;
    }
    else
    {
        this->pipe = p;
    }
}

PipeConnection::PipeConnection(const std::wstring& name, HANDLE pipe) :
name(name),
pipe(pipe)
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
		::CloseHandle(this->pipe);
		this->pipe = NULL;
	}
}

void 
PipeConnection::beginRead(boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onReadComplete,this,_1,_2));
}

void 
PipeConnection::beginRead(boost::shared_ptr<IOMsgReadComplete> msgRead, boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginRead(this->shared_from_this(), boost::bind(&Connection::onReadComplete,this,_1,_2), msgRead);
}

inline
void 
PipeConnection::beginWrite(boost::shared_ptr<IOMsgWriteComplete> msgWrite, 
			   boost::any tag /* = NULL */)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), msgWrite, tag);
}


void 
PipeConnection::beginWrite(boost::shared_array<char> data, unsigned int len, boost::any tag /*= NULL*/)
{
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, len);
}

void 
PipeConnection::beginWrite(std::istream& dataStream, boost::any tag /*= NULL*/)
{
	boost::shared_array<char> data(new char[IOMsgWriteComplete::MAX]);
	dataStream.read(data.get(),IOMsgWriteComplete::MAX);
	int len = dataStream.gcount();
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, len);
}

void 
PipeConnection::beginWrite(const std::string& str, boost::any tag /*= NULL*/)
{
	unsigned int len = str.size();
	boost::shared_array<char> data(new char[len]);
	str.copy(data.get(), len);
	Proactor::instance()->beginWrite(this->shared_from_this(), boost::bind(&Connection::onWriteComplete,this,_1,_2), data, len);
}

std::string 
PipeConnection::toString()
{
	return Util::unicodeToUtf8(this->name);
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
