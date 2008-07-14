#include <cxxtest\TestSuite.h>
#include <boost\bind.hpp>

#include "Indexer.h"
#include "Logger/Logger.h"
#include "ConsoleProgressReporter.h"

#define LOG_SENDER L"IndexerTest"


using namespace Musador;

class IndexerTest : public CxxTest::TestSuite 
{

public:
	
	IndexerTest()
	{
		Logging::Logger::instance();		
	}

	~IndexerTest()
	{
		Logging::Logger::destroy();
	}

	void setUp() 
	{
	}

	void tearDown()
	{
	}

	static void indexDoneSlot(const Musador::IndexerProgress& p)
	{
        LOG(Info) << "Indexing of " << p.numFiles << " files in " << p.numDirs << " directories (" << Util::bytesToString(p.bytes) << ") completed in " << (std::clock() - p.startTime) / CLOCKS_PER_SEC << " seconds";
	}

	void testIndexer() 
	{
		LOG(Info) << " IndexerTest begin...";

		Musador::Indexer indexer(L"testIndexer.db");

		boost::signals::scoped_connection conn = indexer.sigDone.connect(&IndexerTest::indexDoneSlot);
		indexer.clearRootTargets();

//		indexer.addRootTarget(L"C:\\Music\\library\\Classical");
//		indexer.addRootTarget(L"C:\\Music\\library\\Latin");
//		indexer.addRootTarget(L"c:\\music\\library\\Blues");
//		indexer.addRootTarget(L"c:\\music\\library\\Reggae");
//		indexer.addRootTarget(L"c:\\music\\library\\Jazz");
//		indexer.addRootTarget(L"c:\\music");
//		indexer.addRootTarget(L"c:\\Program Files\\Newsleecher\\downloads");

		indexer.reindex();
		indexer.waitDone();	
	
		LOG(Info) << " IndexerTest end...";
	}

};