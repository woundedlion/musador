#include <iostream>
#include "ConsoleProgressReporter.h"
#include "Indexer.h"

using namespace Musador;

ConsoleProgressReporter::ConsoleProgressReporter(const Indexer& indexer) :
indexer(indexer)
{
	
}

void ConsoleProgressReporter::run()
{
	boost::xtime xt;
	boost::xtime_get(&xt,boost::TIME_UTC);
	xt.sec += 1; 
	boost::thread::sleep(xt);
	std::wcout << L'\n';

	IndexerProgress p;
	int i = 0;
	std::wcout << std::endl;
	do 
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

		p = indexer.progress();
		std::wcout << " " << p.numFiles << L" files and " << p.numDirs << L" directories (" << Util::bytesToString(p.bytes) << L") indexed";
		// erase rest of line with padding
		for (int j = 0; j < 20; j++)
			std::wcout << L' ';

		++i %= 4;

		boost::xtime xt;
		boost::xtime_get(&xt,boost::TIME_UTC);
		xt.nsec += 250000000; 
		boost::thread::sleep(xt);

	} while (!p.done);

	std::wcout << std::endl << std::endl;
}

