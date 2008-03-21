#include "GUIConnection.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"HTTPConnection"

using namespace Musador;

GUIConnection::GUIConnection() :
PipeConnection(GUI_PIPE_NAME)
{
}

GUIConnection::GUIConnection(HANDLE pipe) : 
PipeConnection(GUI_PIPE_NAME,pipe)
{

}

void
GUIConnection::onReadComplete(boost::shared_ptr<IOMsg> msg, boost::any tag /* = NULL */)
{

}

void
GUIConnection::onWriteComplete(boost::shared_ptr<IOMsg> msg, boost::any tag /* = NULL */)
{

}