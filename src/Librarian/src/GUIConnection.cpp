#include "GUIConnection.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"HTTPConnection"

using namespace Musador;

GUIConnection::GUIConnection(HANDLE pipe) : 
PipeConnection(GUI_PIPE_NAME,pipe)
{

}