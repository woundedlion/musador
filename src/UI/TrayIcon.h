#ifndef WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E
#define WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

typedef TCHAR* Icon;

namespace Musador
{
    namespace UI
    {
        class TrayIcon
        {
        public:

            TrayIcon(HWND hWnd, UINT uCallbackMsg);

            ~TrayIcon();

            void setIcon(const Icon& icon);

            void setToolTip(const std::wstring& toolTip);

            void show();

            void hide();

            UINT getID();

        private:

            static volatile LONG counter;

            bool visible;
            HWND hWnd;

            NOTIFYICONDATAW nid;

        };
    }
}
#endif