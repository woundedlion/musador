#include "PipeListener.h"
#include "Server/Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;

PipeListener::PipeListener(const std::wstring& name) :
name(name)
{
}

PipeListener::~PipeListener()
{
}

void
PipeListener::beginAccept(EventHandler handler, boost::any tag /* = NULL */)
{
	Proactor::instance()->beginAccept(this->shared_from_this(), handler, tag);
	LOG(Debug) << "Accepting connections on named pipe: " << this->name;
}

std::wstring
PipeListener::getName()
{
	return this->name;
}