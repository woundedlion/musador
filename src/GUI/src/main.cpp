#include "boost/algorithm/string.hpp"
#include <iostream>
#include "LibrarianGUI.h"

using namespace Musador;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{ 
    LibrarianGUI gui;
    gui.run();
    return 0;
}




