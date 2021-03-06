#ifndef LIBRARIANGUI_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARIANGUI_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "UI/App.h"
#include "UI/TrayIcon.h"
#include "UI/Menu.h"
#include "UI/Daemon.h"

#include "Protocol/GUIConnection.h"

#define WM_APP_TRAYICON WM_APP

namespace Musador
{
    class GUIMsg;

    class LibrarianService : public UI::Daemon<LibrarianService>
    {
        friend class UI::Daemon<LibrarianService>;

    public:

        LibrarianService() : Daemon(L"Musador Librarian") {}

        /// @brief Stub method satisfy the Daemon interface. NOT USED.
        int run(unsigned long argc, LPTSTR argv[]) { return 0; }
    };

    class LibrarianGUI : public UI::App
    {
    public:
        LibrarianGUI();
        ~LibrarianGUI();
        LRESULT wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

        void onServiceConnect(boost::shared_ptr<IO::Msg> msg, boost::any tag = NULL);
        void onServiceMsg(boost::shared_ptr<GUIMsg> msg);

        void onTrayMenu();

        template <typename T>
        void notifyService();

        boost::shared_ptr<GUIConnection> service;
        std::auto_ptr<UI::TrayIcon> trayIcon;
        UI::Menu trayMenu;

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