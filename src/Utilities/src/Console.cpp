#include "Console.h"
#include "boost/bind.hpp"
#include <functional>

using namespace Musador;

Console::Console() :
hCon(::GetStdHandle(STD_OUTPUT_HANDLE)),
signalHandler(NULL)
{
    CONSOLE_SCREEN_BUFFER_INFO bi = {0};
    BOOL r = ::GetConsoleScreenBufferInfo(this->hCon, &bi);
    if (r)
    {
        this->oldColor = bi.wAttributes & COLOR_WHITE_HI;
    }	
}

Console::~Console()
{
    this->revert();
}

void Console::revert()
{
    ::SetConsoleTextAttribute(this->hCon,this->oldColor);
    if (NULL != this->signalHandler)
    {
        ::SetConsoleCtrlHandler(this->signalHandler, FALSE);
        this->signalHandler = NULL;
    }
}

void Console::setTextColor(TextColor color)
{
    ::SetConsoleTextAttribute(this->hCon, static_cast<WORD>(color));
}

void Console::setSignalHandler(SignalHandler handler)
{

    if (NULL != this->signalHandler)
    {
        ::SetConsoleCtrlHandler(this->signalHandler, FALSE);
    }
    this->signalHandler = handler;	
    ::SetConsoleCtrlHandler(this->signalHandler, TRUE);
}

