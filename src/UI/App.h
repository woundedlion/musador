#ifndef WINAPP_H_986987979879879
#define WINAPP_H_986987979879879

#include <windows.h>
#include <string>
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"

namespace Musador
{
    namespace UI
    {
        typedef boost::mutex Mutex;
        typedef boost::condition_variable Condition;
        typedef boost::mutex::scoped_lock Guard;

        class App
        {
        public:

            App(const std::wstring& appName);

            virtual ~App();

            void run();

            virtual void onRunning() {}

            operator HWND() { return this->hWndMain; }

        protected:

            std::wstring appName;
            HWND hWndMain;
            HINSTANCE hInst;

            static LRESULT CALLBACK _wndProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
            virtual LRESULT wndProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;


        };
    }
}


#endif