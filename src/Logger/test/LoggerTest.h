#include <cxxtest\TestSuite.h>
#include "boost/bind.hpp"
#include "boost/lexical_cast.hpp"
#include "Logger.h"
          
using namespace Musador;
using namespace Logging;


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

	void testLoggerMT()
	{
		const int BANK_COUNT = 10;
		const int BANK_SIZE = 10;
		const int BANK_DELAY = 0;

		Logger::instance();

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

		Logger::destroy();
	}

	void runLoggingThread(int id)
	{
		Logger& log = *Logger::instance();
		for (int i = 0; i < 10; i++)
		{
			log(Info, boost::lexical_cast<std::wstring,int>(id)) << i ;
		}
	}


	void testLoggerConsole() 
	{
		Logger& log = *Logger::instance();
		log.setLevel(Debug);

		// Empty Sender
		log(Debug) << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		log(Info) << "Testing Console Logger | const std::string: " << std::string("Success");
		log(Warning) << "Testing Console Logger | const wchar_t *: " << L"Success";
		log(Error) <<  "Testing Console Logger | const char *: " << "Success";
		log(Critical) << "Testing Console Logger | wchar_t: " << L'Y';
		log(Debug) << "Testing Console Logger | char: " << 'Y';
		log(Info) << "Testing Console Logger | short: " << (short)123;
		log(Warning) << "Testing Console Logger | int: " << (int)123;
		log(Error) << "Testing Console Logger | long: " << (long)123;
		log(Critical) << "Testing Console Logger | long long: " << (long long)123;
		log(Debug) << "Testing Console Logger | float: " << (float)123.456;
		log(Info) << "Testing Console Logger | double: " << (double)123.456;
		log(Warning) << "Testing Console Logger | bool(T): " << true;
		log(Error) << "Testing Console Logger | bool(F): " << false;

		// Sender
		log(Debug, L"LoggerTest") << "Testing Console Logger | const std::wstring: " << std::wstring(L"Success");
		log(Info, L"LoggerTest") << "Testing Console Logger | const std::string: " << std::string("Success");
		log(Warning, L"LoggerTest") << "Testing Console Logger | const wchar_t *: " << L"Success";
		log(Error, L"LoggerTest") <<  "Testing Console Logger | const char *: " << "Success";
		log(Critical, L"LoggerTest") << "Testing Console Logger | wchar_t: " << L'Y';
		log(Debug, L"LoggerTest") << "Testing Console Logger | char: " << 'Y';
		log(Info, L"LoggerTest") << "Testing Console Logger | short: " << (short)123;
		log(Warning, L"LoggerTest") << "Testing Console Logger | int: " << (int)123;
		log(Error, L"LoggerTest") << "Testing Console Logger | long: " << (long)123;
		log(Critical, L"LoggerTest") << "Testing Console Logger | long long: " << (long long)123;
		log(Debug, L"LoggerTest") << "Testing Console Logger | float: " << (float)123.456;
		log(Info, L"LoggerTest") << "Testing Console Logger | double: " << (double)123.456;
		log(Warning, L"LoggerTest") << "Testing Console Logger | bool(T): " << true;
		log(Error, L"LoggerTest") << "Testing Console Logger | bool(F): " << false;

		Logger::destroy();
	}


	void testLoggerNull() 
	{
		/*
		Logger * log = LoggerNull::instance();

		// Macro Sender
		log(Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		log(Info) << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		log(Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		log(Error) <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		log(Critical) << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		log(Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		log(Info) << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		log(Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		log(Error) << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		log(Critical) << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		log(Debug) << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		log(Info) << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		log(Warning) << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		log(Error) << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		// Empty Tag
		log(Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		log(Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		log(Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		log(Error, L"LoggerTest") <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		log(Critical, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		log(Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		log(Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		log(Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		log(Error, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		log(Critical, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		log(Debug, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		log(Info, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		log(Warning, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		log(Error, L"LoggerTest") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		// Tagged
		log(Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::wstring: " << std::wstring(L"Success");
		log(Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const std::string: " << std::string("Success");
		log(Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | const wchar_t *: " << L"Success";
		log(Error, L"LoggerTest",L"Tag") <<  "Error - YOU SHOULD NOT SEE THIS !!! | const char *: " << "Success";
		log(Critical, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | wchar_t: " << L'Y';
		log(Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | char: " << 'Y';
		log(Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | short: " << (short)123;
		log(Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | int: " << (int)123;
		log(Error, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | long: " << (long)123;
		log(Critical, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | long long: " << (long long)123;
		log(Debug, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | float: " << (float)123.456;
		log(Info, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | double: " << (double)123.456;
		log(Warning, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(T): " << true;
		log(Error, L"LoggerTest",L"Tag") << "Error - YOU SHOULD NOT SEE THIS !!! | bool(F): " << false;

		log(Info) << " LoggerTest end...";

		LoggerNull::destroy();
		*/
		
	}

};