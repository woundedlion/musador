#ifndef GUI_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D
#define GUI_LISTENER_H_BD0CAAF7_87FA_46e9_9917_0AC6E4B3734D

#include "Server/PipeListener.h"
#include "Server/ConnectionProcessor.h"

namespace Musador
{
	class GUIListener : public PipeListener
	{
	public:

		GUIListener();

		boost::shared_ptr<Connection> createConnection();

	};
}

#endif