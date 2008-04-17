#ifndef GUI_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define GUI_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

#include "Server/PipeConnection.h"
#include "GUIMessages.h"

namespace Musador
{
	static const wchar_t * GUI_PIPE_NAME = L"\\\\.\\pipe\\LibrarianGUI";

	class GUIConnection : public PipeConnection
	{

	public:

		GUIConnection();

		void accepted(boost::any tag = NULL) { this->beginRead(); }

		void onReadComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL);

		void onWriteComplete(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL);

		void onConnectComplete(boost::shared_ptr<IOMsg>, boost::any tag = NULL);

		void setHandler(const GUIHandler& handler);

	private:

		GUIHandler handler;

		int connRetries;
	};

}

#endif