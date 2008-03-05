#ifndef LIBRARIANGUI_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARIANGUI_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include "Utilities/WinApp.h"
#include "Utilities/MessageSink.h"
#include "Utilities/WindowsShellIcon.h"

#define WM_APP_SERVERUP WM_APP
#define WM_APP_SERVERDOWN WM_APP + 1
#define WM_APP_TRAYICON WM_APP + 2

namespace Musador
{
	class LibrarianGUI : public WinApp
	{
	public:

		LibrarianGUI();

		~LibrarianGUI();

		HRESULT wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

		WindowsShellIcon trayIcon;
	};
}


#endif