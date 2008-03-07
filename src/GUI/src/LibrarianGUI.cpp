#include "LibrarianGUI.h"
#include "res/resource.h"
#include "Logger/Logger.h"
#define LOG_SENDER "LibrarianGUI"

#include "boost/format.hpp"

using namespace Musador;

LibrarianGUI::LibrarianGUI() :
WinApp(L"Musador Librarian"),
trayIcon(NULL)
{
	this->trayMenu.insertItem(0,ENABLE,L"Enable");
	this->trayMenu.insertSep(1,EXIT_SEP);
	this->trayMenu.insertItem(2,EXIT,L"Exit");
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
		this->trayIcon->setToolTip(L"Musador Librarian : Running");
		this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_ACTIVE));
		this->trayIcon->show();
		this->trayMenu.updateItem(ENABLE,DISABLE,L"Disable");
		break;
	
	case WM_APP_SERVERDOWN:
		this->trayIcon->setToolTip(L"Musador Librarian : Disabled");
		this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
		this->trayIcon->show();
		this->trayMenu.updateItem(DISABLE,ENABLE,L"Enable");
		break;
	
	case WM_APP_TRAYICON:
		if (wParam == this->trayIcon->getID())
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
		case DISABLE:
//			Librarian::instance()->disable();
			break;
		case ENABLE:
//			Librarian::instance()->enable();
			break;
		case EXIT:
			::PostQuitMessage(0);
			break;
		}
		break;

	default:
		::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 1;
}

void 
LibrarianGUI::onRunning()
{
	this->trayIcon.reset(new WindowsShellIcon(this->hWndMain,WM_APP_TRAYICON));
	this->trayIcon->setToolTip(L"Musador Librarian : Disabled");
	this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
	this->trayIcon->show();
}

void
LibrarianGUI::onTrayMenu()
{
	this->trayMenu.popupAtCursor(this->hWndMain);
}