#include "boost/bind.hpp"
#include "PipeConnection.h"
#include "Utilities/Util.h"
#include "Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"
using namespace Musador;
namespace ipc = boost::interprocess;

#pragma warning(push)
#pragma warning(disable: 4355)

PipeConnection::PipeConnection(const std::wstring& name) :
name(name),
pipe(NULL)
{
	// Set up shared mem
	ipc::named_mutex::remove((this->friendlyName() + "Mutex").c_str());
	ipc::named_condition::remove((this->friendlyName() + "Cond").c_str());
	ipc::shared_memory_object::remove((this->friendlyName() + "Listening").c_str());
	this->listeningMutex.reset(new ipc::named_mutex(ipc::open_or_create, (this->friendlyName() + "Mutex").c_str()));
	this->listeningCond.reset(new ipc::named_condition(ipc::open_or_create, (this->friendlyName() + "Cond").c_str()));
	this->listening.reset(new ipc::shared_memory_object(ipc::open_or_create, (this->friendlyName() + "Listening").c_str(), ipc::read_write));
	ipc::scoped_lock<ipc::named_mutex> lock(*this->listeningMutex);
	this->listening->truncate(1);
}

#pragma warning(pop)

PipeConnection::~PipeConnection()
{
	this->close();

	// Cleanup shared mem
	ipc::named_mutex::remove((this->friendlyName() + "Mutex").c_str());
	ipc::named_condition::remove((this->friendlyName() + "Cond").c_str());
	ipc::shared_memory_object::remove((this->friendlyName() + "Listening").c_str());
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
PipeConnection::beginWaitForListener(EventHandler handler, boost::any tag /*= NULL*/)
{
	boost::thread waitingThread(boost::bind(&PipeConnection::waitForListener,this, handler, tag));
	waitingThread.detach();
}

void 
PipeConnection::waitForListener(EventHandler handler, boost::any tag /*= NULL*/)
{
	{
		ipc::scoped_lock<ipc::named_mutex> lock(*this->listeningMutex);
		ipc::mapped_region listeningRegion(*this->listening,ipc::read_only);
		while (*(reinterpret_cast<bool *>(listeningRegion.get_address())) == 0)
		{
			this->listeningCond->wait(lock);
		}
	}

	if (FALSE == ::WaitNamedPipe(this->name.c_str(),INFINITE))
	{
		if (handler)
		{
			// notify the handler
			DWORD err = ::GetLastError();
			boost::shared_ptr<IOMsgError> msgErr(new IOMsgError());
			msgErr->conn = shared_from_this();
			msgErr->err = err;
			handler(msgErr,tag);
		}
		return;
	}

	boost::shared_ptr<IOMsgPipeWaitComplete> msgWait(new IOMsgPipeWaitComplete());
	msgWait->conn = shared_from_this();
	handler(msgWait,tag);
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