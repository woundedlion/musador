#ifndef LIBRARIANGUI_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARIANGUI_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/shared_memory_object.hpp>


#include "Utilities/WinApp.h"
#include "Utilities/MessageSink.h"
#include "Utilities/WindowsShellIcon.h"
#include "Utilities/WinMenu.h"
#include "Utilities/WindowsService.h"

#include "Protocol/GUIConnection.h"

#define WM_APP_TRAYICON WM_APP

namespace Musador
{
	class GUIMsg;
        
        class LibrarianService : public WindowsService<LibrarianService>
        {
            friend class WindowsService<LibrarianService>;
        public:

            LibrarianService() : 
              WindowsService(L"Musador Librarian") {}

            int run(unsigned long argc, LPTSTR argv[]) { return 0; }
        };

	class LibrarianGUI : public WinApp
	{
	public:

		LibrarianGUI();

		~LibrarianGUI();

		HRESULT wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

		enum MenuItem
		{
			EXIT,
			EXIT_SEP,
			ENABLE,
			DISABLE
		};

		void onRunning();
		void connectService();

		void onServiceConnect(boost::shared_ptr<IOMsg> msg, boost::any tag = NULL);
		void onServiceMsg(boost::shared_ptr<GUIMsg> msg);

		void onTrayMenu();

		template <typename T>
		void notifyService();

		boost::shared_ptr<GUIConnection> service;
		std::auto_ptr<WindowsShellIcon> trayIcon;
		WinMenu trayMenu;

	};

	template <typename T>
	void LibrarianGUI::notifyService()
	{
		if (NULL != this->service->getPipe())
		{
			boost::shared_ptr<T> msg(new T());
			std::stringstream msgData;
			boost::archive::binary_oarchive ar(msgData);
			ar & msg;
			this->service->beginWrite(msgData);
		}
	}
}


#endif