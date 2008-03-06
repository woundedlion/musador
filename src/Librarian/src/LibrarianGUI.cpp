#include "Librarian.h"
#include "LibrarianGUI.h"
#include "res/resource.h"
#include "Logger/Logger.h"
#define LOG_SENDER "LibrarianGUI"

#include "boost/format.hpp"

using namespace Musador;

LibrarianGUI::LibrarianGUI() :
WinApp(L"Musador Librarian"),
trayIcon(this->hWndMain, WM_APP_TRAYICON)
{
}

LibrarianGUI::~LibrarianGUI()
{

}

HRESULT LibrarianGUI::wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LOG(Debug) << "wndProcMain received msg: " << (boost::wformat(L"%|#02x|") % uMsg) << " [wParam=" << wParam << "],[lParam=" << lParam << "]";

	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	
	case WM_APP_SERVERUP:
		this->trayIcon.setToolTip(L"Musador Librarian : Active");
		this->trayIcon.setIcon(MAKEINTRESOURCE(IDI_ACTIVE));
		this->trayIcon.show();
		break;
	
	case WM_APP_SERVERDOWN:
		this->trayIcon.hide();
		break;
	
	case WM_APP_TRAYICON:
		if (wParam == this->trayIcon.getID())
		{
			switch (lParam)
			{
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				this->onTrayMenu();
				break;
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_IDM_EXIT:
			Librarian::instance()->stop();
			break;
		}
		break;

	default:
		::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 1;
}

void
LibrarianGUI::onTrayMenu()
{
	HMENU hMenu = ::LoadMenu(this->hInst, MAKEINTRESOURCE(IDM_TRAY));
	HMENU popup = ::GetSubMenu(hMenu,0);
	POINT p;
	::GetCursorPos(&p);
	::SetForegroundWindow(this->hWndMain);
	::TrackPopupMenu(popup, TPM_LEFTALIGN | TPM_BOTTOMALIGN, p.x, p.y, 0, this->hWndMain, NULL);
	::DestroyMenu(hMenu);
}