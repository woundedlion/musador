#ifndef WINDOWSERVICE_07837928_36A9_4351_ABA1_2A355EE4825D
#define WINDOWSERVICE_07837928_36A9_4351_ABA1_2A355EE4825D

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <tchar.h>
#include "Utilities/Singleton.h"
#include "Utilities/Util.h"
#include "Utilities/StreamException.h"

#if defined(UNICODE) || defined(_UNICODE)
#define tstring std::wstring
#else
#define tstring std::string
#endif

namespace Musador
{
    namespace UI
    {
        template <class T>
        class Daemon : public Util::Singleton<T>
        {
        public:

            Daemon(const tstring& svcName);
            ~Daemon();
            void install();
            void uninstall();
            void serviceStart();
            void stop();
            void start();
            void terminate();
            void waitForStop();
            virtual int run(unsigned long argc, LPTSTR argv[]) = 0;
            virtual void ctrl(DWORD opCode) {} // Override this member function to hook user-defined control opcodes

        private:

            tstring svcName;
            SERVICE_STATUS_HANDLE hStatus;
            SERVICE_STATUS status;
            HANDLE evtStop;
            HANDLE evtSrc;
            SC_HANDLE hSvc;

            static void WINAPI runThunk(DWORD argc, LPTSTR argv[])
            {
                T::instance()->_run(argc, argv);
            }

            static void WINAPI ctrlThunk(DWORD opCode)
            {
                T::instance()->_ctrl(opCode);
            }

            void _run(DWORD argc, LPTSTR argv[]);
            void _ctrl(DWORD opCode);
            void setStatus(DWORD dwCurrentState,DWORD dwWin32ExitCode,DWORD dwWaitHint);
            void logEvent(WORD wType, const tstring& msg);
        };

        struct ServiceException : public Util::StreamException<ServiceException>
        {};

        struct ServiceAlreadyStartedException : public ServiceException
        {};

        template <class T>
        Daemon<T>::Daemon(const tstring& svcName) :
        svcName(svcName),
            hSvc(NULL)
        {
            evtStop = ::CreateEvent(NULL,TRUE,FALSE,NULL);
            evtSrc = ::RegisterEventSource(NULL,svcName.c_str());
        }

        template <class T>
        Daemon<T>::~Daemon()
        {
            ::CloseHandle(evtStop);
            ::DeregisterEventSource(evtSrc);
        }

        template <class T>
        void Daemon<T>::install()
        {
            SC_HANDLE schSCManager;
            TCHAR szPath[MAX_PATH];

            if (!GetModuleFileName( NULL, szPath, MAX_PATH )){
                throw ServiceException() << "Cannot install service: " << ::GetLastError();
            }

            // Get a handle to the SCM database.  
            schSCManager = ::OpenSCManager( 
                NULL,                    // local computer
                NULL,                    // ServicesActive database 
                SC_MANAGER_ALL_ACCESS);  // full access rights 

            if (NULL == schSCManager) {
                throw ServiceException() << "OpenSCManager failed: " << ::GetLastError();
            }

            // Create the service
            hSvc = ::CreateService( 
                schSCManager,              // SCM database 
                svcName.c_str(),     // name of service 
                svcName.c_str(),		// service name to display 
                SERVICE_ALL_ACCESS,        // desired access 
                SERVICE_WIN32_OWN_PROCESS, // service type 
                SERVICE_AUTO_START,      // start type 
                SERVICE_ERROR_NORMAL,      // error control type 
                szPath,                    // path to service's binary 
                NULL,                      // no load ordering group 
                NULL,                      // no tag identifier 
                NULL,                      // no dependencies 
                NULL,                      // LocalSystem account 
                NULL);                     // no password 


            if (hSvc == NULL) {
                ::CloseServiceHandle(schSCManager);
                throw ServiceException() << "CreateService failed: " << ::GetLastError();
            }

            try {
                start();
            }
            catch (const ServiceException& e) {
                logEvent(EVENTLOG_INFORMATION_TYPE,Util::utf8ToUnicode(e.what()));
            }

            ::CloseServiceHandle(hSvc); 
            ::CloseServiceHandle(schSCManager);
        }

        template <class T>
        void Daemon<T>::uninstall()
        {
            SC_HANDLE schSCManager;
            TCHAR szPath[MAX_PATH];

            if(!GetModuleFileName( NULL, szPath, MAX_PATH )) {
                throw ServiceException() << "Cannot uninstall service: " << ::GetLastError();
            }

            // Get a handle to the SCM database.  
            schSCManager = ::OpenSCManager( 
                NULL,                    // local computer
                NULL,                    // ServicesActive database 
                SC_MANAGER_ALL_ACCESS);  // full access rights 

            if (NULL == schSCManager) 
            {
                throw ServiceException() << "OpenSCManager failed: " << ::GetLastError();
            }

            // Delete the service
            hSvc = ::OpenService( 
                schSCManager,           // SCM database 
                svcName.c_str(),  // name of service 
                DELETE | SERVICE_STOP );				// access

            if (hSvc == NULL) 
            {
                ::CloseServiceHandle(schSCManager);
                throw ServiceException() << "The specified service is not installed. (" << ::GetLastError() << ")";
            }

            // Stop the service
            try {
                terminate();
            }
            catch (const ServiceException& e) {
                logEvent(EVENTLOG_INFORMATION_TYPE,Util::utf8ToUnicode(e.what()));
            }

            BOOL r = ::DeleteService(hSvc);

            ::CloseServiceHandle(hSvc); 
            ::CloseServiceHandle(schSCManager);

            if (!r) {
                throw ServiceException() << "The specified service could not be deleted. (" << ::GetLastError() << ")";
            }

            return;
        }

        template <class T>
        void Daemon<T>::serviceStart()
        {
            SERVICE_TABLE_ENTRY dispatchTable[] = 
            { 
                { const_cast<wchar_t *>(svcName.c_str()),&Daemon::runThunk}, 
                { NULL, NULL } 
            }; 

            if (!::StartServiceCtrlDispatcher( dispatchTable )) 
            { 
                DWORD err = ::GetLastError();
                throw ServiceException() << "StartServiceCtrlDispatcher() failed: " << err; 
            } 
        }

        template <class T>
        void Daemon<T>::stop()
        {
            ::SetEvent(evtStop);
        }

        template <class T>
        void Daemon<T>::start()
        {
            if (NULL == hSvc) {
                SC_HANDLE schSCManager;
                TCHAR szPath[MAX_PATH];

                if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
                {
                    throw ServiceException() << "Cannot launch service: " << ::GetLastError();
                }

                // Get a handle to the SCM database.  
                schSCManager = ::OpenSCManager( 
                    NULL,                    // local computer
                    NULL,                    // ServicesActive database 
                    SC_MANAGER_ALL_ACCESS);  // full access rights 

                if (NULL == schSCManager)  {
                    throw ServiceException() << "OpenSCManager failed: " << ::GetLastError();
                }

                // Open the service
                hSvc = ::OpenService( 
                    schSCManager,           // SCM database 
                    svcName.c_str(),  // name of service 
                    SERVICE_START );	// access

                if (hSvc == NULL) {
                    ::CloseServiceHandle(schSCManager);
                    throw ServiceException() << "The specified service is not installed. (" << ::GetLastError() << ")";
                }

            }

            // Start the Service through the SCM
            if (0 == ::StartService(hSvc, 0, NULL)) {
                if (::GetLastError() == ERROR_SERVICE_ALREADY_RUNNING) {
                    throw ServiceAlreadyStartedException();
                } else {
                    throw ServiceException() << "The service could not be started. (" << ::GetLastError() << ")";
                }
            }
        }

        template <class T>
        void Daemon<T>::terminate()
        {
            // Stop the Service through the SCM
            SERVICE_STATUS st;
            if (0 == ::ControlService(hSvc, SERVICE_CONTROL_STOP, &st)) {
                throw ServiceException() << "The service could not be stopped. (" << ::GetLastError() << ")";
            }
        }

        template <class T>
        void Daemon<T>::waitForStop()
        {
            ::WaitForSingleObject(evtStop,INFINITE);
        }

        template <class T>
        void Daemon<T>::_run(DWORD argc, LPTSTR argv[])
        {
            hStatus = ::RegisterServiceCtrlHandler(svcName.c_str(),&Daemon::ctrlThunk);
            if (!hStatus) {
                throw ServiceException() << "RegisterServiceCtrlHandler() failed: " << ::GetLastError();
            }
            status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
            status.dwServiceSpecificExitCode = 0;
            setStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

            ::ResetEvent(evtStop);

            setStatus(SERVICE_RUNNING, NO_ERROR, 0);
            unsigned long err = run(argc, argv);
            if (0 == err)
                setStatus(SERVICE_STOPPED, NO_ERROR, 0);
            else
                setStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 0);
        }

        template <class T>
        void Daemon<T>::_ctrl(DWORD opCode)
        {
            switch(opCode) 
            {
            case SERVICE_CONTROL_STOP:
                setStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
                stop();
                ctrl(opCode);
                return;
            case SERVICE_CONTROL_INTERROGATE: 
            default:
                ctrl(opCode);
                break;
            } 

            setStatus(status.dwCurrentState, NO_ERROR, 0);
        }

        template <class T>
        void Daemon<T>::setStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
        {
            static DWORD dwCheckPoint = 1;

            status.dwCurrentState = dwCurrentState;
            status.dwWin32ExitCode = dwWin32ExitCode;
            status.dwWaitHint = dwWaitHint;

            if (dwCurrentState == SERVICE_START_PENDING)
                status.dwControlsAccepted = 0;
            else status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

            if ( (dwCurrentState == SERVICE_RUNNING) ||
                (dwCurrentState == SERVICE_STOPPED) )
                status.dwCheckPoint = 0;
            else status.dwCheckPoint = dwCheckPoint++;

            ::SetServiceStatus( hStatus, &status );
        }

        template <class T>
        void Daemon<T>::logEvent(WORD wType, const tstring& msg)
        {
            const TCHAR * message = msg.c_str();
            ::ReportEvent(	evtSrc,
                wType,	// Type
                0,		// Category
                NULL,	// event ID
                NULL,   // sid
                1,		// Num Strings
                0,		// raw data size
                &message,	// strings
                NULL) ;	// rawdata
        }
    }
}
#endif