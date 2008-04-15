#ifndef WINDOWSERVICE_07837928_36A9_4351_ABA1_2A355EE4825D
#define WINDOWSERVICE_07837928_36A9_4351_ABA1_2A355EE4825D


#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include "Singleton.h"
#include "Util.h"
#include "StreamException.h"

#if defined(UNICODE) || defined(_UNICODE)
#define tstring std::wstring
#else
#define tstring std::string
#endif

template <class T>
class WindowsService : public Singleton<T>
{
public:
	
	WindowsService(const tstring& svcName);
	
	~WindowsService();

	void install();

	void uninstall();

	void start();

	void stop();

	void launch();

	void terminate();

	static void WINAPI runThunk(DWORD argc, LPTSTR argv[])
	{
		T::instance()->_run(argc, argv);
	}

	static void WINAPI ctrlThunk(DWORD opCode)
	{
		T::instance()->_ctrl(opCode);
	}

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

	void _run(DWORD argc, LPTSTR argv[]);
	void _ctrl(DWORD opCode);
	void setStatus(DWORD dwCurrentState,DWORD dwWin32ExitCode,DWORD dwWaitHint);
	void logEvent(WORD wType, const tstring& msg);
};

class ServiceException : public StreamException<ServiceException>
{
public:

	ServiceException() {}

	~ServiceException() {}

};

template <class T>
WindowsService<T>::WindowsService(const tstring& svcName) :
svcName(svcName),
hSvc(NULL)
{
	this->evtStop = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	this->evtSrc = ::RegisterEventSource(NULL,this->svcName.c_str());
}

template <class T>
WindowsService<T>::~WindowsService()
{
	::CloseHandle(this->evtStop);
	::DeregisterEventSource(this->evtSrc);
}

template <class T>
void WindowsService<T>::install()
{
    SC_HANDLE schSCManager;
    TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
		throw ServiceException() << "Cannot install service: " << ::GetLastError();
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

    // Create the service
	this->hSvc = ::CreateService( 
        schSCManager,              // SCM database 
        this->svcName.c_str(),     // name of service 
        this->svcName.c_str(),		// service name to display 
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
 
	
    if (this->hSvc == NULL) 
    {
		::CloseServiceHandle(schSCManager);
		throw ServiceException() << "CreateService failed: " << ::GetLastError();
    }
	
	// Start the service
	try
	{
		this->launch();
	}
	catch (const ServiceException& e)
	{
		this->logEvent(EVENTLOG_INFORMATION_TYPE,Util::utf8ToUnicode(e.what()));
	}

	
	::CloseServiceHandle(this->hSvc); 
	::CloseServiceHandle(schSCManager);
}

template <class T>
void WindowsService<T>::uninstall()
{
    SC_HANDLE schSCManager;
    TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
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
	this->hSvc = ::OpenService( 
        schSCManager,           // SCM database 
        this->svcName.c_str(),  // name of service 
		DELETE | SERVICE_STOP );				// access

    if (this->hSvc == NULL) 
    {
		::CloseServiceHandle(schSCManager);
		throw ServiceException() << "The specified service is not installed. (" << ::GetLastError() << ")";
    }

	// Stop the service
	try
	{
		this->terminate();
	}
	catch (const ServiceException& e)
	{
		this->logEvent(EVENTLOG_INFORMATION_TYPE,Util::utf8ToUnicode(e.what()));
	}

	BOOL r = ::DeleteService(this->hSvc);

	::CloseServiceHandle(this->hSvc); 
	::CloseServiceHandle(schSCManager);

	if (!r)
	{
		throw ServiceException() << "The specified service could not be deleted. (" << ::GetLastError() << ")";
	}

	return;
}

template <class T>
void WindowsService<T>::start()
{
		SERVICE_TABLE_ENTRY dispatchTable[] = 
		{ 
			{ const_cast<wchar_t *>(this->svcName.c_str()),&WindowsService::runThunk}, 
			{ NULL, NULL } 
		}; 
	 
		if (!::StartServiceCtrlDispatcher( dispatchTable )) 
		{ 
			DWORD err = ::GetLastError();
			throw ServiceException() << "StartServiceCtrlDispatcher() failed: " << err; 
		} 
}

template <class T>
void WindowsService<T>::stop()
{
	::SetEvent(this->evtStop);
}

template <class T>
void WindowsService<T>::launch()
{
        if (NULL == this->hSvc)
        {
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

            if (NULL == schSCManager) 
            {
                throw ServiceException() << "OpenSCManager failed: " << ::GetLastError();
            }

            // Open the service
            this->hSvc = ::OpenService( 
                schSCManager,           // SCM database 
                this->svcName.c_str(),  // name of service 
                SERVICE_START );	// access

            if (this->hSvc == NULL) 
            {
                ::CloseServiceHandle(schSCManager);
                throw ServiceException() << "The specified service is not installed. (" << ::GetLastError() << ")";
            }

        }

        // Start the Service through the SCM
	if (0 == ::StartService(this->hSvc, 0, NULL))
	{
		throw ServiceException() << "The service could not be started. (" << ::GetLastError() << ")";
	}
}

template <class T>
void WindowsService<T>::terminate()
{
	// Stop the Service through the SCM
	SERVICE_STATUS st;
	if (0 == ::ControlService(this->hSvc, SERVICE_CONTROL_STOP, &st))
	{
		throw ServiceException() << "The service could not be stopped. (" << ::GetLastError() << ")";
	}
}

template <class T>
void WindowsService<T>::waitForStop()
{
	::WaitForSingleObject(this->evtStop,INFINITE);
}

template <class T>
void WindowsService<T>::_run(DWORD argc, LPTSTR argv[])
{
	this->hStatus = ::RegisterServiceCtrlHandler(this->svcName.c_str(),&WindowsService::ctrlThunk);
	if (!this->hStatus)
	{
		throw ServiceException() << "RegisterServiceCtrlHandler() failed: " << ::GetLastError();
	}
	this->status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	this->status.dwServiceSpecificExitCode = 0;
	this->setStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	::ResetEvent(this->evtStop);

	this->setStatus(SERVICE_RUNNING, NO_ERROR, 0);
	unsigned long err = this->run(argc, argv);
	if (0 == err)
		this->setStatus(SERVICE_STOPPED, NO_ERROR, 0);
	else
		this->setStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 0);
}

template <class T>
void WindowsService<T>::_ctrl(DWORD opCode)
{
	switch(opCode) 
	{
	case SERVICE_CONTROL_STOP:
		this->setStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		this->stop();
		this->ctrl(opCode);
		return;
	case SERVICE_CONTROL_INTERROGATE: 
	default:
		this->ctrl(opCode);
		break;
   } 

   this->setStatus(this->status.dwCurrentState, NO_ERROR, 0);
}

template <class T>
void WindowsService<T>::setStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	this->status.dwCurrentState = dwCurrentState;
    this->status.dwWin32ExitCode = dwWin32ExitCode;
    this->status.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        this->status.dwControlsAccepted = 0;
    else this->status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ( (dwCurrentState == SERVICE_RUNNING) ||
           (dwCurrentState == SERVICE_STOPPED) )
        this->status.dwCheckPoint = 0;
    else this->status.dwCheckPoint = dwCheckPoint++;

	::SetServiceStatus( this->hStatus, &this->status );
}

template <class T>
void WindowsService<T>::logEvent(WORD wType, const tstring& msg)
{
	const TCHAR * message = msg.c_str();
	::ReportEvent(	this->evtSrc,
					wType,	// Type
                    0,		// Category
                    NULL,	// event ID
                    NULL,   // sid
                    1,		// Num Strings
                    0,		// raw data size
                    &message,	// strings
                    NULL) ;	// rawdata
}

#endif