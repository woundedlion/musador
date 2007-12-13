#include <cxxtest\TestSuite.h>
#include "Logger.h"
          

class LoggerTest : public CxxTest::TestSuite 
{

public:
	
	LoggerTest()
	{
		
	}

	~LoggerTest()
	{

	}

	void setUp() 
	{
	}

	void tearDown()
	{
	}

	void testLoggerConsole() 
	{
		Musador::Logger * logger = Musador::LoggerConsole::instance();

		// Macro Sender
		logger->log(Musador::Debug) << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		logger->log(Musador::Info) << "Testing Console Logger | const std::string: " << std::string("Success");
		logger->log(Musador::Warning) << "Testing Console Logger | const wchar_t *: " << L"Success";
		logger->log(Musador::Error) <<  "Testing Console Logger | const char *: " << "Success";
		logger->log(Musador::Critical) << "Testing Console Logger | wchar_t: " << L'Y';
		logger->log(Musador::Debug) << "Testing Console Logger | char: " << 'Y';
		logger->log(Musador::Info) << "Testing Console Logger | short: " << (short)123;
		logger->log(Musador::Warning) << "Testing Console Logger | int: " << (int)123;
		logger->log(Musador::Error) << "Testing Console Logger | long: " << (long)123;
		logger->log(Musador::Critical) << "Testing Console Logger | long long: " << (long long)123;
		logger->log(Musador::Debug) << "Testing Console Logger | float: " << (float)123.456;
		logger->log(Musador::Info) << "Testing Console Logger | double: " << (double)123.456;
		logger->log(Musador::Warning) << "Testing Console Logger | bool(T): " << true;
		logger->log(Musador::Error) << "Testing Console Logger | bool(F): " << false;

		// Empty Tag
		logger->log(Musador::Debug, L"LoggerTest") << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		logger->log(Musador::Info, L"LoggerTest") << "Testing Console Logger | const std::string: " << std::string("Success");
		logger->log(Musador::Warning, L"LoggerTest") << "Testing Console Logger | const wchar_t *: " << L"Success";
		logger->log(Musador::Error, L"LoggerTest") <<  "Testing Console Logger | const char *: " << "Success";
		logger->log(Musador::Critical, L"LoggerTest") << "Testing Console Logger | wchar_t: " << L'Y';
		logger->log(Musador::Debug, L"LoggerTest") << "Testing Console Logger | char: " << 'Y';
		logger->log(Musador::Info, L"LoggerTest") << "Testing Console Logger | short: " << (short)123;
		logger->log(Musador::Warning, L"LoggerTest") << "Testing Console Logger | int: " << (int)123;
		logger->log(Musador::Error, L"LoggerTest") << "Testing Console Logger | long: " << (long)123;
		logger->log(Musador::Critical, L"LoggerTest") << "Testing Console Logger | long long: " << (long long)123;
		logger->log(Musador::Debug, L"LoggerTest") << "Testing Console Logger | float: " << (float)123.456;
		logger->log(Musador::Info, L"LoggerTest") << "Testing Console Logger | double: " << (double)123.456;
		logger->log(Musador::Warning, L"LoggerTest") << "Testing Console Logger | bool(T): " << true;
		logger->log(Musador::Error, L"LoggerTest") << "Testing Console Logger | bool(F): " << false;

		// Tagged
		logger->log(Musador::Debug, L"LoggerTest",L"Tag") << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		logger->log(Musador::Info, L"LoggerTest",L"Tag") << "Testing Console Logger | const std::string: " << std::string("Success");
		logger->log(Musador::Warning, L"LoggerTest",L"Tag") << "Testing Console Logger | const wchar_t *: " << L"Success";
		logger->log(Musador::Error, L"LoggerTest",L"Tag") <<  "Testing Console Logger | const char *: " << "Success";
		logger->log(Musador::Critical, L"LoggerTest",L"Tag") << "Testing Console Logger | wchar_t: " << L'Y';
		logger->log(Musador::Debug, L"LoggerTest",L"Tag") << "Testing Console Logger | char: " << 'Y';
		logger->log(Musador::Info, L"LoggerTest",L"Tag") << "Testing Console Logger | short: " << (short)123;
		logger->log(Musador::Warning, L"LoggerTest",L"Tag") << "Testing Console Logger | int: " << (int)123;
		logger->log(Musador::Error, L"LoggerTest",L"Tag") << "Testing Console Logger | long: " << (long)123;
		logger->log(Musador::Critical, L"LoggerTest",L"Tag") << "Testing Console Logger | long long: " << (long long)123;
		logger->log(Musador::Debug, L"LoggerTest",L"Tag") << "Testing Console Logger | float: " << (float)123.456;
		logger->log(Musador::Info, L"LoggerTest",L"Tag") << "Testing Console Logger | double: " << (double)123.456;
		logger->log(Musador::Warning, L"LoggerTest",L"Tag") << "Testing Console Logger | bool(T): " << true;
		logger->log(Musador::Error, L"LoggerTest",L"Tag") << "Testing Console Logger | bool(F): " << false;

		Musador::LoggerConsole::destroy();
	}

	void testLoggerNull() 
	{
		Musador::Logger * logger = Musador::LoggerNull::instance();

		// Macro Sender
		logger->log(Musador::Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		logger->log(Musador::Info) << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		logger->log(Musador::Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		logger->log(Musador::Error) <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		logger->log(Musador::Critical) << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		logger->log(Musador::Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		logger->log(Musador::Info) << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		logger->log(Musador::Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		logger->log(Musador::Error) << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		logger->log(Musador::Critical) << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		logger->log(Musador::Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		logger->log(Musador::Info) << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		logger->log(Musador::Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		logger->log(Musador::Error) << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		// Empty Tag
		logger->log(Musador::Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		logger->log(Musador::Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		logger->log(Musador::Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		logger->log(Musador::Error, L"LoggerTest") <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		logger->log(Musador::Critical, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		logger->log(Musador::Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		logger->log(Musador::Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		logger->log(Musador::Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		logger->log(Musador::Error, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		logger->log(Musador::Critical, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		logger->log(Musador::Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		logger->log(Musador::Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		logger->log(Musador::Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		logger->log(Musador::Error, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		// Tagged
		logger->log(Musador::Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		logger->log(Musador::Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		logger->log(Musador::Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		logger->log(Musador::Error, L"LoggerTest",L"Tag") <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		logger->log(Musador::Critical, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		logger->log(Musador::Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		logger->log(Musador::Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		logger->log(Musador::Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		logger->log(Musador::Error, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		logger->log(Musador::Critical, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		logger->log(Musador::Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		logger->log(Musador::Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		logger->log(Musador::Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		logger->log(Musador::Error, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		logger->log(Musador::Info) << " LoggerTest end...";

		Musador::LoggerNull::destroy();
		
	}

};