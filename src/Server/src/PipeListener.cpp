#include "PipeListener.h"
#include "Server/Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;

PipeListener::PipeListener(const std::wstring& name) :
pipe(NULL),
name(name)
{
}

PipeListener::~PipeListener()
{
	this->close();
}

void
PipeListener::close()
{
	if (NULL != this->pipe)
	{
		::CloseHandle(this->pipe);
		this->pipe = NULL;
	}
}

void
PipeListener::beginAccept(EventHandler handler, boost::any tag /* = NULL */)
{
	if (NULL != this->pipe)
	{
		::DisconnectNamedPipe(this->pipe);
	}

	HANDLE p = ::CreateNamedPipe(this->name.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 
		PIPE_UNLIMITED_INSTANCES,
		4096,
		4096,
		INFINITE,
		NULL
		);

	if (INVALID_HANDLE_VALUE == p)
	{
		LOG(Error) << "CreateNamedPipe() Failed: " << ::GetLastError();
		return;
	}
	else
	{
		this->pipe = p;
	}

	Proactor::instance()->beginAccept(this->shared_from_this(), handler, tag);
	LOG(Debug) << "Accepting connections on named pipe: " << this->name;
}

