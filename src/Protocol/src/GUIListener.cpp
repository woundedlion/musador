#include "GUIListener.h"
#include "GUIConnection.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;

GUIListener::GUIListener() :
PipeListener(GUI_PIPE_NAME)
{}

boost::shared_ptr<IO::Connection>
GUIListener::createConnection()
{
    boost::shared_ptr<GUIConnection> conn(new GUIConnection());
    return conn;
}