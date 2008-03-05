#include "WinApp.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"WinApp"
using namespace Musador;

WinApp::WinApp(const std::wstring& appName) :
appName(appName),
hWndMain(NULL),
hInst(::GetModuleHandle(NULL))
{
	WNDCLASSEX wndClass = {0};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = NULL;
	wndClass.lpfnWndProc = &WinApp::_wndProc;
	wndClass.hInstance = this->hInst;
	wndClass.lpszClassName = this->appName.c_str();
	ATOM atom = ::RegisterClassEx(&wndClass);

	if (0 == atom)
	{
		LOG(Error) << "Could not register main window class [" << ::GetLastError() << "]";
		return;
	}

	this->hWndMain = ::CreateWindowEx(	NULL,
										this->appName.c_str(),
										this->appName.c_str(),
										WS_POPUP,
										0,
										0,
										0,
										0,
										NULL,
										NULL,
										this->hInst,
                                                                                this
									  );

	if (NULL == this->hWndMain)
	{
		LOG(Error) << "Could not create main window instance [" << ::GetLastError() << "]";
		return;
	}
}

WinApp::~WinApp()
{
	::DestroyWindow(this->hWndMain);
	::UnregisterClass(this->appName.c_str(),this->hInst);
}

LRESULT WinApp::_wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{
        case WM_NCCREATE:
                ::SetWindowLongPtr(hWnd,GWL_USERDATA, (LONG)reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
	default:
                WinApp * app = reinterpret_cast<WinApp *>(::GetWindowLongPtr(hWnd,GWL_USERDATA));
		if (NULL != app)
		{
			return app->wndProcMain(hWnd,uMsg,wParam,lParam);
		}
                return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 1;
}

LRESULT WinApp::wndProcMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LOG(Debug) << "wndProcMain received msg: " << uMsg << " [wParam=" << wParam << "],[lParam=" << lParam << "]";
    return 1;
}