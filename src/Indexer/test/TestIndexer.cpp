#include "UnitTest++.h"
#include "Logger/Logger.h"
#include "Utilities/Util.h"
#include "Indexer/Indexer.h"
#include "Indexer/ConsoleProgressReporter.h"
#include "IO/Proactor.h"

#define LOG_SENDER L"TestIndexer"

using namespace Musador;

TEST(testIndexer)
{
	Indexer indexer(L"test.db");
	indexer.addTarget(L"D:\\Music");
	indexer.reindex();

    ConsoleProgressReporter reporter(indexer);
    reporter.run();

    IndexerProgress p = indexer.getProgress();
    unsigned int duration = (std::clock() - p.startTime) / CLOCKS_PER_SEC;
    LOG(Info) << "\n" "Indexing of " << p.numFiles << " files in " << p.numDirs << " directories (" 
		<< Util::bytesToString(p.bytes) << ") completed in " << duration << " seconds";
}


int main()
{
	Logger::instance()->setLevel(Musador::Info);
	IO::Proactor::instance()->start();
	auto err = UnitTest::RunAllTests();
	IO::Proactor::instance()->stop();
	IO::Proactor::instance()->destroy();
	Logger::instance()->destroy();
	return err;
}