#include <cxxtest\TestSuite.h>

#include "Util.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"UtilTest"

using namespace Musador;

class UtilTest : public CxxTest::TestSuite 
{

public:
	
	UtilTest()
	{
		Musador::Logger::instance();		
	}

	~UtilTest()
	{
		Musador::Logger::destroy();
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
	}

	void tearDown()
	{
	}
};