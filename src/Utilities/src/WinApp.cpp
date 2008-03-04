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
	wndClass.style = WS_POPUP;
	wndClass.lpfnWndProc = &WinApp::_wndProc;
	wndClass.hInstance = this->hInst;
	wndClass.hIcon = NULL;
	wndClass.lpszClassName = this->appName.c_str();
	ATOM atom = ::RegisterClassExW(&wndClass);

	if (0 == atom)
	{
		LOG(Error) << "Could not register main window class [" << ::GetLastError() << "]";
		return;
	}

	this->hWndMain = ::CreateWindowExW(	NULL,
										this->appName.c_str(),
										this->appName.c_str(),
										WS_POPUP,
										0,
										0,
										0,
										0,
										NULL,
										NULL,
										this->hInst,NULL
									  );

	if (NULL == this->hWndMain)
	{
		LOG(Error) << "Could not create main window class [" << ::GetLastError() << "]";
		return;
	}
}

WinApp::~WinApp()
{
	::DestroyWindow(this->hWndMain);
	::UnregisterClass(this->appName.c_str(),this->hInst);
}

HRESULT WinApp::_wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{
	case WM_INITDIALOG:
		SetWindowLong(hWnd, GWL_USERDATA, (long)lParam);
	default:
		WinApp * app = reinterpret_cast<WinApp *>(GetWindowLongPtr(hWnd, GWL_USERDATA));
		if (NULL != app)
		{
			return app->wndProcMain(hWnd,uMsg,wParam,lParam);
		}
		return -1;
	}
	return 1;
}