#include <boost/bind.hpp>

#include "Protocol/GUIListener.h"
#include "Protocol/GUIMessages.h"
#include "Server/IOMessages.h"
#include "Server/Proactor.h"
#include "Utilities/TimerQueue.h"
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
	Proactor::instance()->start();
	TimerQueue::instance()->start();

	this->trayMenu.insertItem(0,ENABLE,L"Enable");
	this->trayMenu.insertSep(1,EXIT_SEP);
	this->trayMenu.insertItem(2,EXIT,L"Exit");
}

LibrarianGUI::~LibrarianGUI()
{
    Proactor::instance()->stop();
    Proactor::destroy();
	TimerQueue::instance()->stop();
	TimerQueue::destroy();
}

HRESULT LibrarianGUI::wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LOG(Debug) << "wndProcMain received msg: " << (boost::wformat(L"%|#02x|") % uMsg) << " [wParam=" << wParam << "],[lParam=" << lParam << "]";

	switch (uMsg)
	{

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_APP_TRAYICON:
		if (wParam == this->trayIcon->getID())
		{
			switch (lParam)
			{
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
				this->onTrayMenu();
				break;
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case DISABLE:
			this->trayMenu.updateItem(DISABLE,DISABLE,L"Disable",false);
			this->notifyService<GUIMsgDisableReq>();
			break;
		case ENABLE:
			{
				this->trayMenu.updateItem(ENABLE,ENABLE,L"Enable",false);
				LibrarianService l;
				try
				{
					l.start();
				}
				catch (const ServiceAlreadyStartedException&)
				{
					this->service->beginConnect(boost::bind(&LibrarianGUI::onServiceConnect,this, _1, _2));
					break;
				}
			}
			break;
		case EXIT:
			::PostQuitMessage(0);
			break;
		}
		break;

	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void 
LibrarianGUI::onRunning()
{
	this->trayIcon.reset(new WindowsShellIcon(this->hWndMain,WM_APP_TRAYICON));
	this->trayIcon->setToolTip(L"Musador Librarian : Disabled");
	this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
	this->trayIcon->show();

	this->service.reset(new GUIConnection());
	this->service->setHandler(boost::bind(&LibrarianGUI::onServiceMsg,this,_1));
	this->service->beginConnect(boost::bind(&LibrarianGUI::onServiceConnect,this, _1, _2));

}

void
LibrarianGUI::onTrayMenu()
{
	this->trayMenu.popupAtCursor(this->hWndMain);
}

void
LibrarianGUI::onServiceConnect(boost::shared_ptr<IOMsg> msg, boost::any tag /* = NULL*/)
{
	switch (msg->getType())
	{
	case IO_PIPE_CONNECT_COMPLETE:
		this->service->beginRead();
		break;
	case IO_ERROR:
		break;
	}
}

void
LibrarianGUI::onServiceMsg(boost::shared_ptr<GUIMsg> msg)
{
    switch (msg->getType())
    {
	case GUI_MSG_ENABLED_NOTIFY:
		this->trayIcon->setToolTip(L"Musador Librarian : Running");
		this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_ACTIVE));
		this->trayIcon->show();
		this->trayMenu.updateItem(ENABLE,DISABLE,L"Disable");
		break;

	case GUI_MSG_DISABLED_NOTIFY:
		this->trayIcon->setToolTip(L"Musador Librarian : Disabled");
		this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
		this->trayIcon->show();
		this->trayMenu.updateItem(DISABLE,ENABLE,L"Enable");
		break;
    }
}
