#include "boost/algorithm/string.hpp"
#include <iostream>
#include "Librarian.h"
#include "UI/Console.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"Librarian"

using namespace Musador;

BOOL WINAPI sigHandler(DWORD ctrl)
{
    switch(ctrl)
    {
    case CTRL_C_EVENT:
        Librarian::instance()->stop();
        break;
    }
    return TRUE;
}

void usage()
{
    std::cout << "Usage: " <<std::endl;
    std::cout << "Librarian /?" << std::endl;
    std::cout << "Librarian /install" << std::endl;
    std::cout << "Librarian /uninstall" << std::endl;
    std::cout << "Librarian /noservice" << std::endl;
    std::cout << "Librarian /index outfile path [path ...]" << std::endl;
}

int _tmain(int argc, wchar_t * argv[]) 
{ 
#if _DEBUG
    Logging::Logger::instance()->setLevel(Logging::Debug);
#else
    Logging::Logger::instance()->setLevel(Logging::Info);
#endif

    int r = 0;
    try
    {
        Librarian * app = Librarian::instance();
        if (argc > 1)
        {
            if(boost::iequals(argv[1],L"/install"))
            {
                LOG(Info) << L"Installing Librarian Service...";
                app->install();
                LOG(Info) << L"Librarian Service successfully installed.";
            }
            else if (boost::iequals(argv[1],L"/uninstall"))
            {
                LOG(Info) << L"Uninstalling Librarian Service...";
                app->uninstall();
                LOG(Info) << L"Librarian Service successfully uninstalled.";
            }
            else if (boost::iequals(argv[1],L"/noservice"))
            {
                UI::Console console;
                console.setSignalHandler(&sigHandler);
                app->run(argc, argv);
            }
            else if (boost::iequals(argv[1],L"/index"))
            {
                if (argc < 4)
                {
                    usage();
                    return -1;
                }
                std::wstring outfile = argv[2];
                std::vector<std::wstring> paths;
                for (int i = 3; i < argc; ++i)
                {
                    paths.push_back(argv[i]);
                }

                app->index(outfile,paths);
            }
            else if(boost::iequals(argv[1],L"/?"))
            {
                usage();
                return 0;
            }
            else
            {
                usage();
                return -1;
            }
        } 
        else
        {
            app->serviceStart();
            r = 0;
        }

        Librarian::destroy();	
    }
    catch (const UI::ServiceException& e)
    {
        LOG(Error) << e.what();
        r = -1;
    }

    Logging::Logger::destroy();
    return r;
}




