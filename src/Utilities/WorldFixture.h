#include <iostream>
#include <string>
#include <cxxtest\GlobalFixture.h>

namespace Util
{
    class WorldFixture: public CxxTest::GlobalFixture
    {	
        bool tearDownWorld()
        {
            std::string r;
            std::cout << std::endl << "Press <enter> to continue...";
            std::getline(std::cin,r);
            return true;
        }
    };
}