#include <cxxtest\TestSuite.h>

#include "Util.h"
#include "Logger/Logger.h"
#define LOG(LVL) Musador::LoggerConsole::instance()->log(Musador::LVL,"UtilTest")

class UtilTest : public CxxTest::TestSuite 
{

public:
	
	UtilTest()
	{
		
	}

	~UtilTest()
	{

	}

	void testEscapeQuotes()
	{
		LOG(Info) << "Util::esacpeQuotes test begin...";
		TS_ASSERT(Util::escapeQuotes(L"") == L"");
		TS_ASSERT(Util::escapeQuotes(L"'") == L"''");
		TS_ASSERT(Util::escapeQuotes(L"abcdef") == L"abcdef");
		TS_ASSERT(Util::escapeQuotes(L"abcdef's") == L"abcdef''s");
		TS_ASSERT(Util::escapeQuotes(L"abcdef'") == L"abcdef''");
		TS_ASSERT(Util::escapeQuotes(L"'abcdef") == L"''abcdef");
		TS_ASSERT(Util::escapeQuotes(L"'TESTING''TESTING'") == L"''TESTING''''TESTING''");
		LOG(Info) << "Util::escapeQuotes test end...";
	}

	void setUp() 
	{
		Musador::LoggerConsole::instance();
	}

	void tearDown()
	{
		Musador::LoggerConsole::destroy();
	}
};