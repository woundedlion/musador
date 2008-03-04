#include "WindowsShellIcon.h"

WindowsShellIcon::counter = 0;

WindowsShellIcon::WindowsShellIcon(HWND hWnd) :
visible(false),
hWnd(hWnd),
uID(::InterlockedIncrement(WindowsShellIcon::counter))
{

}

void WindowsShellIcon::setIcon(const std::string& iconName);
void WindowsShellIcon::setToolTip(const std::wstring& toolTip);
void WindowsShellIcon::show();
void WindowsShellIcon::hide();
