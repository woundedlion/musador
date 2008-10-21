#include <boost/bind.hpp>

#include "Protocol/GUIListener.h"
#include "Protocol/GUIMessages.h"
#include "IO/IOMessages.h"
#include "IO/Proactor.h"
#include "Utilities/TimerQueue.h"
#include "LibrarianGUI.h"
#include "res/resource.h"
#include "Logger/Logger.h"
#define LOG_SENDER "LibrarianGUI"

#include "boost/format.hpp"

using namespace Musador;

LibrarianGUI::LibrarianGUI() :
UI::WinApp(L"Musador Librarian"),
trayIcon(NULL)
{
    IO::Proactor::instance()->start();
    Util::TimerQueue::instance()->start();

    this->trayMenu.insertItem(0,ENABLE,L"Enable");
    this->trayMenu.insertSep(1,EXIT_SEP);
    this->trayMenu.insertItem(2,EXIT,L"Exit");
}

LibrarianGUI::~LibrarianGUI()
{
    IO::Proactor::instance()->stop();
    IO::Proactor::destroy();
    Util::TimerQueue::instance()->stop();
    Util::TimerQueue::destroy();
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
                catch (const UI::ServiceAlreadyStartedException&)
                {}
                catch(const UI::ServiceException&)
                {
                    //TODO: LOG!
                    break;
                }
                this->service->beginConnect();
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
    this->trayIcon.reset(new UI::WindowsShellIcon(this->hWndMain,WM_APP_TRAYICON));
    this->trayIcon->setToolTip(L"Musador Librarian : Disabled");
    this->trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
    this->trayIcon->show();

    this->service.reset(new GUIConnection());
    this->service->setHandler(boost::bind(&LibrarianGUI::onServiceMsg,this,_1));
    this->service->beginConnect();
}

void
LibrarianGUI::onTrayMenu()
{
    this->trayMenu.popupAtCursor(this->hWndMain);
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
