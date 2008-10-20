#include "boost/algorithm/string.hpp"
#include <iostream>
#include "LibrarianGUI.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{ 
    Musador::LibrarianGUI gui;
    gui.run();
    return 0;
}




