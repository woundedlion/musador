#ifndef WINAPP_H_986987979879879
#define WINAPP_H_986987979879879

#include <winsock2.h>
#include <windows.h>
#include <string>


class WinApp
{
public:

	WinApp(const std::wstring& appName);

	~WinApp();

	static HRESULT CALLBACK _wndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual HRESULT wndProcMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	operator HWND() { return this->hWndMain; }

private:

	std::wstring appName;
	HWND hWndMain;
	HINSTANCE hInst;
};

#endif