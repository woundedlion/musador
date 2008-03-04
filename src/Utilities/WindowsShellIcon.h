#ifndef WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E
#define WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E

#include <string>
#include <WinSock2.h>
#include <shellapi.h>
#include <tchar.h>

typedef TCHAR* Icon;

class WindowsShellIcon
{
public:

    WindowsShellIcon(HWND hWnd);
    
    void setIcon(const Icon& icon);
    void setToolTip(const std::wstring& toolTip);
    void show();
    void hide();

private:

    static volatile LONG counter;

    bool visible;
    HWND hWnd;

	NOTIFYICONDATAW nid;
	
};

#endif