#include <cxxtest\TestSuite.h>
#include "boost/bind.hpp"
#include "boost/lexical_cast.hpp"
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
		Musador::Logger& log = *Musador::Logger::instance();

		// Macro Sender
		log(Musador::Debug) << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		log(Musador::Info) << "Testing Console Logger | const std::string: " << std::string("Success");
		log(Musador::Warning) << "Testing Console Logger | const wchar_t *: " << L"Success";
		log(Musador::Error) <<  "Testing Console Logger | const char *: " << "Success";
		log(Musador::Critical) << "Testing Console Logger | wchar_t: " << L'Y';
		log(Musador::Debug) << "Testing Console Logger | char: " << 'Y';
		log(Musador::Info) << "Testing Console Logger | short: " << (short)123;
		log(Musador::Warning) << "Testing Console Logger | int: " << (int)123;
		log(Musador::Error) << "Testing Console Logger | long: " << (long)123;
		log(Musador::Critical) << "Testing Console Logger | long long: " << (long long)123;
		log(Musador::Debug) << "Testing Console Logger | float: " << (float)123.456;
		log(Musador::Info) << "Testing Console Logger | double: " << (double)123.456;
		log(Musador::Warning) << "Testing Console Logger | bool(T): " << true;
		log(Musador::Error) << "Testing Console Logger | bool(F): " << false;

		// Empty Tag
		log(Musador::Debug, L"LoggerTest") << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		log(Musador::Info, L"LoggerTest") << "Testing Console Logger | const std::string: " << std::string("Success");
		log(Musador::Warning, L"LoggerTest") << "Testing Console Logger | const wchar_t *: " << L"Success";
		log(Musador::Error, L"LoggerTest") <<  "Testing Console Logger | const char *: " << "Success";
		log(Musador::Critical, L"LoggerTest") << "Testing Console Logger | wchar_t: " << L'Y';
		log(Musador::Debug, L"LoggerTest") << "Testing Console Logger | char: " << 'Y';
		log(Musador::Info, L"LoggerTest") << "Testing Console Logger | short: " << (short)123;
		log(Musador::Warning, L"LoggerTest") << "Testing Console Logger | int: " << (int)123;
		log(Musador::Error, L"LoggerTest") << "Testing Console Logger | long: " << (long)123;
		log(Musador::Critical, L"LoggerTest") << "Testing Console Logger | long long: " << (long long)123;
		log(Musador::Debug, L"LoggerTest") << "Testing Console Logger | float: " << (float)123.456;
		log(Musador::Info, L"LoggerTest") << "Testing Console Logger | double: " << (double)123.456;
		log(Musador::Warning, L"LoggerTest") << "Testing Console Logger | bool(T): " << true;
		log(Musador::Error, L"LoggerTest") << "Testing Console Logger | bool(F): " << false;

		// Tagged
		log(Musador::Debug, L"LoggerTest",L"Tag") << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		log(Musador::Info, L"LoggerTest",L"Tag") << "Testing Console Logger | const std::string: " << std::string("Success");
		log(Musador::Warning, L"LoggerTest",L"Tag") << "Testing Console Logger | const wchar_t *: " << L"Success";
		log(Musador::Error, L"LoggerTest",L"Tag") <<  "Testing Console Logger | const char *: " << "Success";
		log(Musador::Critical, L"LoggerTest",L"Tag") << "Testing Console Logger | wchar_t: " << L'Y';
		log(Musador::Debug, L"LoggerTest",L"Tag") << "Testing Console Logger | char: " << 'Y';
		log(Musador::Info, L"LoggerTest",L"Tag") << "Testing Console Logger | short: " << (short)123;
		log(Musador::Warning, L"LoggerTest",L"Tag") << "Testing Console Logger | int: " << (int)123;
		log(Musador::Error, L"LoggerTest",L"Tag") << "Testing Console Logger | long: " << (long)123;
		log(Musador::Critical, L"LoggerTest",L"Tag") << "Testing Console Logger | long long: " << (long long)123;
		log(Musador::Debug, L"LoggerTest",L"Tag") << "Testing Console Logger | float: " << (float)123.456;
		log(Musador::Info, L"LoggerTest",L"Tag") << "Testing Console Logger | double: " << (double)123.456;
		log(Musador::Warning, L"LoggerTest",L"Tag") << "Testing Console Logger | bool(T): " << true;
		log(Musador::Error, L"LoggerTest",L"Tag") << "Testing Console Logger | bool(F): " << false;

		Musador::Logger::destroy();
	}

	void testLoggerMT()
	{
		const int BANK_COUNT = 10;
		const int BANK_SIZE = 10;
		const int BANK_DELAY = 0;

		Musador::Logger::instance();

		boost::thread * loggingThreads[BANK_COUNT * BANK_SIZE];
		for (int i = 0; i < BANK_COUNT; ++i)
		{
			for (int j = 0; j < BANK_SIZE; j++)
			{
				loggingThreads[BANK_SIZE*i + j] = new boost::thread(boost::bind(&LoggerTest::runLoggingThread,this,BANK_SIZE*i+j));
			}
			::Sleep(BANK_DELAY);
		}
		for (int i = 0; i < BANK_COUNT * BANK_SIZE; ++i)
		{
			loggingThreads[i]->join();
			delete loggingThreads[i];
		}
	
		Musador::Logger::destroy();
	}

	void runLoggingThread(int id)
	{
		Musador::Logger& log = *Musador::Logger::instance();
		for (int i = 0; i < 10; i++)
		{
			log(Musador::Info, boost::lexical_cast<std::wstring,int>(id)) << i ;
		}
	}

	void testLoggerNull() 
	{
		/*
		Musador::Logger * log = Musador::LoggerNull::instance();

		// Macro Sender
		log(Musador::Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		log(Musador::Info) << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		log(Musador::Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		log(Musador::Error) <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		log(Musador::Critical) << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		log(Musador::Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		log(Musador::Info) << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		log(Musador::Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		log(Musador::Error) << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		log(Musador::Critical) << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		log(Musador::Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		log(Musador::Info) << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		log(Musador::Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		log(Musador::Error) << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		// Empty Tag
		log(Musador::Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		log(Musador::Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		log(Musador::Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		log(Musador::Error, L"LoggerTest") <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		log(Musador::Critical, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		log(Musador::Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		log(Musador::Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		log(Musador::Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		log(Musador::Error, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		log(Musador::Critical, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		log(Musador::Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		log(Musador::Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		log(Musador::Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		log(Musador::Error, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		// Tagged
		log(Musador::Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		log(Musador::Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		log(Musador::Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		log(Musador::Error, L"LoggerTest",L"Tag") <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		log(Musador::Critical, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		log(Musador::Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		log(Musador::Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		log(Musador::Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		log(Musador::Error, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		log(Musador::Critical, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		log(Musador::Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		log(Musador::Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		log(Musador::Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		log(Musador::Error, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		log(Musador::Info) << " LoggerTest end...";

		Musador::LoggerNull::destroy();
		*/
		
	}

};