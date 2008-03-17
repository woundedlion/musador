#ifndef NAMED_PIPE_D0D77706_18C0_4a05_BBAD_41A7F8B529E7
#define NAMED_PIPE_D0D77706_18C0_4a05_BBAD_41A7F8B529E7

#include <winsock2.h>
#include <windows.h>
#include <string>
#include "Utilities/StreamException.h"

namespace Musador
{
	class NamedPipe
	{
	public:

		NamedPipe(const std::wstring& name, 
				  int openMode = PIPE_ACCESS_DUPLEX, 
				  int pipeMode = FILE_FLAG_OVERLAPPED | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS,
				  unsigned int inBufSize = 4096,
				  unsigned int outBufSize = 4096
				  );

		~NamedPipe();

	private:

		HANDLE pipe;

	};

	class PipeException : public StreamException<PipeException>
	{};
}

#endif
