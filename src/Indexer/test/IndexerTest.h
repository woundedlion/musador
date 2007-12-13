#include <cxxtest\TestSuite.h>
#include <boost\bind.hpp>

#include "Indexer.h"
#include "Logger/Logger.h"
#define LOG(LVL) Musador::LoggerConsole::instance()->log(Musador::LVL,"IndexerTest")

class IndexerTest : public CxxTest::TestSuite 
{

public:
	
	IndexerTest()
	{
		
	}

	~IndexerTest()
	{

	}

	void setUp() 
	{
		Musador::LoggerConsole::instance();
	}

	void tearDown()
	{
		Musador::LoggerConsole::destroy();
	}

	static void indexDoneSlot(const Musador::IndexerProgress& p)
	{
		LOG(Info) << "Indexing of " << p.numFiles() << " files in " << p.numDirs() << " directories (" << Util::bytesToString(p.bytes()) << ") completed in " << p.duration() << " seconds";
	}

	void monitorIndexer(const Musador::Indexer * indexer)
	{
		boost::xtime xt;
		boost::xtime_get(&xt,boost::TIME_UTC);
		xt.sec += 1; 
		boost::thread::sleep(xt);
		std::wcout << L'\n';
		
		Musador::IndexerProgress p;
		int i = 0;
		while (indexer->progress(&p))
		{
			// erase previous line
			std::wcout << L"\r ";
			switch (i)
			{
			case 0:
				std::wcout << L'|';
				break;
			case 1:
				std::wcout << L'/';
				break;
			case 2:
				std::wcout << L'-';
				break;
			case 3:
				std::wcout << L'\\';
				break;
			}
			std::wcout << " " << p.numFiles() << " files and " << p.numDirs() << " directories (" << Util::bytesToString(p.bytes()) << ") indexed";
			// erase rest of line with padding
			for (int j = 0; j < 20; j++)
				std::wcout << L' ';

			++i %= 4;

			boost::xtime xt;
			boost::xtime_get(&xt,boost::TIME_UTC);
			xt.nsec += 250000000; 
			boost::thread::sleep(xt);
		}
	}

	void testIndexer() 
	{
		LOG(Info) << " IndexerTest begin...";

		Musador::Indexer indexer(L"testIndexer.db");
		indexer.initDB();

		boost::thread mon(boost::bind(&IndexerTest::monitorIndexer,this,&indexer));
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
		mon.join();
		LOG(Info) << " IndexerTest end...";
	}

};