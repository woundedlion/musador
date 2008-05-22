#include "PipeListener.h"
#include "Server/Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;
namespace ipc = boost::interprocess;

PipeListener::PipeListener(const std::wstring& name) :
name(name)
{
	// Clean up existing shared mem
	ipc::named_mutex::remove((this->friendlyName() + "Mutex").c_str());
	ipc::named_condition::remove((this->friendlyName() + "Cond").c_str());
	ipc::shared_memory_object::remove((this->friendlyName() + "Listening").c_str());
	
	// Set up new shared mem
	this->listeningMutex.reset(new ipc::named_mutex(ipc::open_or_create, (this->friendlyName() + "Mutex").c_str()));
	ipc::scoped_lock<ipc::named_mutex> lock(*this->listeningMutex);
	this->listeningCond.reset(new ipc::named_condition(ipc::open_or_create, (this->friendlyName() + "Cond").c_str()));
	this->listening.reset(new ipc::shared_memory_object(ipc::open_or_create, (this->friendlyName() + "Listening").c_str(), ipc::read_write));
	this->listening->truncate(1);
	ipc::mapped_region listeningRegion(*this->listening, ipc::read_write);
	::memset(listeningRegion.get_address(), false, listeningRegion.get_size());
}

PipeListener::~PipeListener()
{
	ipc::scoped_lock<ipc::named_mutex> lock(*this->listeningMutex);
	ipc::mapped_region listeningRegion(*this->listening,ipc::read_write);
	::memset(listeningRegion.get_address(),false ,listeningRegion.get_size());

	// Cleanup
	ipc::named_mutex::remove((this->friendlyName() + "Mutex").c_str());
	ipc::named_condition::remove((this->friendlyName() + "Cond").c_str());
	ipc::shared_memory_object::remove((this->friendlyName() + "Listening").c_str());
}

void
PipeListener::beginAccept(EventHandler handler, boost::any tag /* = NULL */)
{
	Proactor::instance()->beginAccept(this->shared_from_this(), handler, tag);
	LOG(Debug) << "Accepting connections on named pipe: " << this->name;

	ipc::scoped_lock<ipc::named_mutex> lock(*this->listeningMutex);
	ipc::mapped_region listeningRegion(*this->listening,ipc::read_write);
	::memset(listeningRegion.get_address(),true ,listeningRegion.get_size());
	this->listeningCond->notify_all();
}

std::wstring
PipeListener::getName()
{
	return this->name;
}

std::string
PipeListener::friendlyName()
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