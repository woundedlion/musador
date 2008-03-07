#ifndef WINAPP_H_986987979879879
#define WINAPP_H_986987979879879

#include <winsock2.h>
#include <windows.h>
#include <string>
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"

namespace Musador
{
	typedef boost::mutex Mutex;
	typedef boost::condition Condition;
	typedef boost::mutex::scoped_lock Guard;

	class WinApp
	{
	public:

		WinApp(const std::wstring& appName);

		virtual ~WinApp();

		void run();

		BOOL postMessage(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);

		static HRESULT CALLBACK _wndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual HRESULT wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		virtual void onRunning() {}

		operator HWND() { return this->hWndMain; }

	protected:

		std::wstring appName;
		HWND hWndMain;
		HINSTANCE hInst;

	};

	inline BOOL WinApp::postMessage(UINT uMsg, WPARAM wParam /* = NULL */, LPARAM lParam /* = NULL */)
	{
		return ::PostMessage(this->hWndMain, uMsg, wParam, lParam);
	}
}


#endif