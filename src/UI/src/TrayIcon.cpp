#include "TrayIcon.h"
#include <shlwapi.h>
#pragma comment(lib, "shell32.lib")

#include "Logger/Logger.h"
#define LOG_SENDER L"GUI"
using namespace Musador;

using namespace Musador::UI;

volatile LONG TrayIcon::counter = 0;

TrayIcon::TrayIcon(HWND hWnd, UINT uCallbackMsg) :
visible(false),
hWnd(hWnd)
{
    ::ZeroMemory(&this->nid,sizeof(this->nid));

    // Determine supported version of ShellIcon interface
    this->nid.cbSize = sizeof(NOTIFYICONDATAW_V1_SIZE); // default to lowest version
    HINSTANCE hDll = ::LoadLibraryW(L"shell32.dll");
    if (NULL != hDll)
    {
        DLLGETVERSIONPROC procDllGetVersion;
        procDllGetVersion = (DLLGETVERSIONPROC)::GetProcAddress(hDll, "DllGetVersion");
        if (NULL != procDllGetVersion)
        {
            DLLVERSIONINFO dvi = {0};
            dvi.cbSize = sizeof(dvi);
            HRESULT hr = (*procDllGetVersion)(&dvi);
            if (SUCCEEDED(hr))
            {
                if (dvi.dwMajorVersion < 5)
                {
                    // default is OK
                }
#if _WIN32_IE >= 0x0600
                else if (dvi.dwMajorVersion < 6)
                {
                    this->nid.cbSize = sizeof(NOTIFYICONDATAW_V2_SIZE); // 5.0 < version < 6.0					
                }
#endif
                else
                {
                    this->nid.cbSize = sizeof(NOTIFYICONDATAW); //  6.0 < version
                }
            }
            else
            {
                LOG(Warning) << "Could not determine shell32.dll version. defaulting to < 5.0";
            }
        }
        else
        {
            LOG(Warning) << "Could not determine shell32.dll version. defaulting to < 5.0";
        }
        ::FreeLibrary(hDll);
    }
    else
    {
        LOG(Warning) << "Could not determine shell32.dll version. defaulting to < 5.0";
    }

    this->nid.hWnd = hWnd;
    this->nid.uID = ::InterlockedIncrement(&TrayIcon::counter);
    this->nid.hIcon = ::LoadIcon(NULL,IDI_APPLICATION);
    this->nid.uCallbackMessage = uCallbackMsg;
}

TrayIcon::~TrayIcon()
{
    this->hide();
}

void TrayIcon::setIcon(const Icon& icon)
{
    HICON hIcon = ::LoadIcon(::GetModuleHandle(NULL),icon);
    if (NULL == hIcon)
    {
        LOG(Error) << "Unable to load icon " << icon << " [" << ::GetLastError() << "]";
    }
    else
    {
        this->nid.hIcon = hIcon;
    }
}

void TrayIcon::setToolTip(const std::wstring& toolTip)
{
    DWORD tipSize;
    if (this->nid.cbSize == sizeof(NOTIFYICONDATAW_V1_SIZE))
    {
        tipSize = 64;
    }
    else
    {
        tipSize = 128;
    }
    tipSize = toolTip.copy(this->nid.szTip, tipSize - 1);
    this->nid.szTip[tipSize] = L'\0';
}

void TrayIcon::show()
{
    this->nid.uFlags = NIF_MESSAGE;

    if (NULL != this->nid.hIcon)
    {
        this->nid.uFlags |= NIF_ICON;
    }
    this->nid.uFlags |= NIF_TIP;

    if (::Shell_NotifyIconW(this->visible ? NIM_MODIFY : NIM_ADD, &this->nid))
    {
        this->visible = true;
    }
}

void TrayIcon::hide()
{
    if (this->visible)
    {
        this->nid.uFlags = 0;
        ::Shell_NotifyIconW(NIM_DELETE, &this->nid);
        this->visible = false;
    }
}


UINT
TrayIcon::getID()
{
    return this->nid.uID;
}