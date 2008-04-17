#include <cxxtest\TestSuite.h>

#include <boost/bind.hpp>

#include "Util.h"
#include "Utilities/TimerQueue.h"
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

	int timerTicks;

	void gotTimer()
	{
		LOG(Info) << "Got Timer";
		++timerTicks;
	}

	void testTimerQueue()
	{
		TimerQueue::instance()->start();
		this->timerTicks = 0;	
		TimerQueue::instance()->createTimer(1000, boost::bind(&UtilTest::gotTimer,this),false,123);
		::Sleep(4500);
		TS_ASSERT(this->timerTicks == 4);
		TimerQueue::instance()->stop();
		TimerQueue::destroy();
	}
};