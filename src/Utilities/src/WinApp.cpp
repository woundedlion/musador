#include "WinApp.h"
#include "boost/bind.hpp"
#include "boost/format.hpp"

#include "Logger/Logger.h"
#define LOG_SENDER L"GUI"
using namespace Musador;

#include <assert.h>

WinApp::WinApp(const std::wstring& appName) :
appName(appName),
hWndMain(NULL),
hInst(::GetModuleHandle(NULL)),
started(false)
{
	this->mainThread = new boost::thread(boost::bind(&WinApp::run,this));

	Guard lock(this->startedMutex);
	while (!this->started) this->startedCV.wait(lock);
}

WinApp::~WinApp()
{
	::PostMessage(this->hWndMain,WM_QUIT,NULL,NULL);
	this->mainThread->join();
	delete this->mainThread;
}

void 
WinApp::run()
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

	// Notify waiting threads that we have started
	{
		Guard lock(this->startedMutex);
		this->started = true;
		this->startedCV.notify_all();
	}

	// Main message loop
	BOOL r;
	MSG msg;

	while (0 != (r = ::GetMessage(&msg, this->hWndMain, 0, 0 )))
	{ 
		if (r == -1)
		{
			LOG(Error) << "Main message loop failed. [" << ::GetLastError() << "]";
			break;
		}
		else
		{
			::TranslateMessage(&msg); 
			::DispatchMessage(&msg); 
		}
	} 

	// The main message loop has terminated
	{
		Guard lock(this->startedMutex);
		this->started = false;
	}

	::DestroyWindow(this->hWndMain);
	::UnregisterClass(this->appName.c_str(),this->hInst);
}

LRESULT 
WinApp::_wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT 
WinApp::wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LOG(Debug) << "wndProcMain received msg: " << (boost::wformat(L"%|#02x|") % uMsg) << " [wParam=" << wParam << "],[lParam=" << lParam << "]";
	return 1;
}