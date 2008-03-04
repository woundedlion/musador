#ifndef WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E
#define WINDOWS_SHELL_ICON_H_2103FF09_F0C1_42d8_9B39_EEDC1EB6878E

#include <shellapi.h>

class WindowsShellIcon
{
public:

    WindowsShellIcon(HWND hWnd);
    
    void setIcon(const std::string& iconName);
    void setToolTip(const std::wstring& toolTip);
    void show();
    void hide();

private:

    static counter;

    bool visible;
    HWND hWnd;
    DWORD uID;
    std::string icon;
    std::wstring toolTip;
};

#endif