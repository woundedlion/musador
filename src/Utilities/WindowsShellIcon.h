#ifndef WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E
#define WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E

#include <string>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

typedef TCHAR* Icon;

class WindowsShellIcon
{
public:

    WindowsShellIcon(HWND hWnd, UINT uCallbackMsg);

    ~WindowsShellIcon();

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

#endif