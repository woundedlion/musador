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
UI::App(L"Musador Librarian"),
trayIcon(NULL)
{
    IO::Proactor::instance()->start();
    Util::TimerQueue::instance()->start();

    trayMenu.insertItem(0,ENABLE,L"Enable");
    trayMenu.insertSep(1,EXIT_SEP);
    trayMenu.insertItem(2,EXIT,L"Exit");
}

LibrarianGUI::~LibrarianGUI()
{
    IO::Proactor::instance()->stop();
    IO::Proactor::destroy();
    Util::TimerQueue::instance()->stop();
    Util::TimerQueue::destroy();
}

LRESULT LibrarianGUI::wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LOG(Debug) << "wndProcMain received msg: " << (boost::wformat(L"%|#02x|") % uMsg) << " [wParam=" << wParam << "],[lParam=" << lParam << "]";

    switch (uMsg)
    {

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    case WM_APP_TRAYICON:
        if (wParam == trayIcon->getID())
        {
            switch (lParam)
            {
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
                onTrayMenu();
                break;
            }
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case DISABLE:
            trayMenu.updateItem(DISABLE,DISABLE,L"Disable",false);
            notifyService<GUIMsgDisableReq>();
            break;
        case ENABLE:
            {
                trayMenu.updateItem(ENABLE,ENABLE,L"Enable",false);
                LibrarianService l;
                try
                {
                    l.start();
                }
                catch (const UI::ServiceAlreadyStartedException&)
                {}
                catch(const UI::ServiceException& e)
                {
                    LOG(Error) << "Unable to start service: " << e.what();
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
    trayIcon.reset(new UI::TrayIcon(hWndMain,WM_APP_TRAYICON));
    trayIcon->setToolTip(L"Musador Librarian : Disabled");
    trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
    trayIcon->show();

    service.reset(new GUIConnection());
    service->setHandler(boost::bind(&LibrarianGUI::onServiceMsg,this,_1));
    service->beginConnect();
}

void
LibrarianGUI::onTrayMenu()
{
    trayMenu.popupAtCursor(hWndMain);
}

void
LibrarianGUI::onServiceMsg(boost::shared_ptr<GUIMsg> msg)
{
    switch (msg->getType())
    {
    case GUI_MSG_ENABLED_NOTIFY:
        trayIcon->setToolTip(L"Musador Librarian : Running");
        trayIcon->setIcon(MAKEINTRESOURCE(IDI_ACTIVE));
        trayIcon->show();
        trayMenu.updateItem(ENABLE,DISABLE,L"Disable");
        break;

    case GUI_MSG_DISABLED_NOTIFY:
        trayIcon->setToolTip(L"Musador Librarian : Disabled");
        trayIcon->setIcon(MAKEINTRESOURCE(IDI_INACTIVE));
        trayIcon->show();
        trayMenu.updateItem(DISABLE,ENABLE,L"Enable");
        break;
    }
}
