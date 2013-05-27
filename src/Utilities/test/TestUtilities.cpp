#include <UnitTest++\UnitTest++.h>

#include <boost/bind.hpp>

#include "Utilities/Util.h"
#include "Utilities/TimerQueue.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"UtilTest"

using namespace Musador;

TEST(EscapeQuotes)
{
	LOG(Info) << "Util::esacpeQuotes test begin...";
	CHECK(Util::escapeQuotes(L"") == L"");
	CHECK(Util::escapeQuotes(L"'") == L"''");
	CHECK(Util::escapeQuotes(L"abcdef") == L"abcdef");
	CHECK(Util::escapeQuotes(L"abcdef's") == L"abcdef''s");
	CHECK(Util::escapeQuotes(L"abcdef'") == L"abcdef''");
	CHECK(Util::escapeQuotes(L"'abcdef") == L"''abcdef");
	CHECK(Util::escapeQuotes(L"'TESTING''TESTING'") == L"''TESTING''''TESTING''");
	LOG(Info) << "Util::escapeQuotes test end...";
}

int timerTicks;

void gotTimer()
{
	LOG(Info) << "Got Timer";
	++timerTicks;
}

TEST(TimerQueue)
{
	Util::TimerQueue::instance()->start();
	timerTicks = 0;	
	Util::TimerQueue::instance()->createTimer(1000, boost::bind(&gotTimer),false,123);
	::Sleep(4500);
	CHECK(timerTicks == 4);
	Util::TimerQueue::instance()->stop();
	Util::TimerQueue::destroy();
}

int main()
{
	Musador::Logging::Logger::instance()->setLevel(Musador::Logging::Info);
	auto err = UnitTest::RunAllTests();
	Musador::Logging::Logger::instance()->destroy();
	return err;
}