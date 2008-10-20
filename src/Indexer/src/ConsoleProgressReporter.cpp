#include <iostream>
#include <iomanip>

#include "ConsoleProgressReporter.h"
#include "Indexer.h"

using namespace Musador;

ConsoleProgressReporter::ConsoleProgressReporter(const Indexer& indexer) :
indexer(indexer)
{

}

void ConsoleProgressReporter::run()
{
    int i = 0;
    IndexerProgress p;
    do 
    {
        // Get the progress
        IndexerProgress lastP = p;
        p = indexer.progress();

        if (p.bytes > 0)
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

            std::wcout << L" " << p.numFiles << L" files and " << p.numDirs << L" directories (" << Util::bytesToString(p.bytes) << L") indexed";

            // Print speed
            double duration = static_cast<double>(p.curTime - lastP.curTime) / CLOCKS_PER_SEC;
            unsigned int bytes = static_cast<int>(p.bytes - lastP.bytes);
            if (duration > 0 && !p.done)
            {
                std::wcout << L" (" << std::fixed << std::setprecision(2) << Util::bytesToString(static_cast<int>(bytes / duration)) << L"/sec" << L")";
            }

            // erase rest of line with padding
            for (int j = 0; j < 20; j++)
                std::wcout << L' ';

            ++i %= 4;
        }

        boost::xtime xt;
        boost::xtime_get(&xt,boost::TIME_UTC);
        xt.nsec += 250000000; 
        boost::thread::sleep(xt);
    } while (!p.done);

    std::wcout << std::endl;
}

