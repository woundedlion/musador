#include "boost/algorithm/string.hpp"
#include <iostream>
#include "Librarian.h"
#include "Utilities/Console.h"

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

int _tmain(int argc, wchar_t * argv[]) 
{ 
	int r = 0;
	try
	{
		Librarian * app = Librarian::instance();
		if (argc > 1)
		{
			if(boost::iequals(argv[1],L"install"))
			{
				app->install();
				r = 0;
			}
			else if (boost::iequals(argv[1],L"uninstall"))
			{
				app->uninstall();
				r = 0;
			}
			else if (boost::iequals(argv[1],L"noservice"))
			{
				Console console;
				console.setSignalHandler(&sigHandler);
				app->run(argc, argv);
				r = 0;
			}
		} 
		else
		{
			app->start();
			r = 0;
		}

		Librarian::destroy();	
	}
	catch (const ServiceException& e)
	{
		std::cerr << e.what();
		r = -1;
	}
	return r;
}




