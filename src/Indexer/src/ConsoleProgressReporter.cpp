#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

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
        p = indexer.getProgress();
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
            double duration = static_cast<double>(p.curTime - p.startTime) / CLOCKS_PER_SEC;
            if (duration > 0 && !p.done)
            {
				std::wcout << L" (" << std::fixed << std::setprecision(2) << p.numFiles / duration << L" files/sec" << L")";
            }

            // erase rest of line with padding
            for (int j = 0; j < 20; j++)
                std::wcout << L' ';

            ++i %= 4;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    } while (!p.done);

    std::wcout << std::endl;
}

