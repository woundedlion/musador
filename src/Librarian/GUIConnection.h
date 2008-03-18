#ifndef GUI_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define GUI_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "Server/PipeConnection.h"

namespace Musador
{
	static const wchar_t * GUI_PIPE_NAME = L"\\\\.\\pipe\\LibrarianGUI";

	class GUIConnection : public PipeConnection
	{

	public:

		GUIConnection(HANDLE pipe);

		void accepted() { this->beginRead(); }

		void post(boost::shared_ptr<IOMsgReadComplete> msgRead) 
		{ 
			this->beginWrite(msgRead->buf,msgRead->len);
		}

		void post(boost::shared_ptr<IOMsgWriteComplete> msgWrite) 
		{
			this->beginRead();
		}
	};

}

#endif