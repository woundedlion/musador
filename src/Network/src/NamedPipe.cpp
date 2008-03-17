#include "NamedPipe.h"

using namespace Musador;

NamedPipe::NamedPipe(const std::wstring& name, 
					 int openMode /* = PIPE_ACCESS_DUPLEX */, 
					 int pipeMode /* = FILE_FLAG_OVERLAPPED | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS */, 
					 unsigned int inBufSize /* = 4096 */, 
					 unsigned int outBufSize /* = 4096 */ ) :
pipe(NULL)
{
	HANDLE p = ::CreateNamedPipe(name.c_str(),
								openMode, 
								pipeMode, 
								PIPE_UNLIMITED_INSTANCES,
								inBufSize,
								outBufSize,
								INFINITE,
								NULL
								);
	if (INVALID_HANDLE_VALUE == p)
	{
		throw PipeException() << "CreateNamedPipe() Failed: " << ::GetLastError();
	}
	else
	{
		this->pipe = p;
	}
}

NamedPipe::~NamedPipe()
{
	if (NULL != this->pipe)
	{
		::CloseHandle(this->pipe);
	}
}