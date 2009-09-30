#include "PipeListener.h"
#include "IO/Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;
using namespace Musador::IO;

PipeListener::PipeListener(const std::wstring& name) :
name(name)
{
}

PipeListener::~PipeListener()
{
}

void
PipeListener::beginAccept(EventHandler handler, boost::any tag /* = NULL */)
{
    Proactor::instance()->beginAccept(this->shared_from_this(), handler, tag);
    LOG(Debug) << "Accepting connections on named pipe: " << this->name;
}

std::wstring
PipeListener::getName()
{
    return this->name;
}

std::string 
PipeListener::toString()
{
    return Util::unicodeToUtf8(this->getName());
}

