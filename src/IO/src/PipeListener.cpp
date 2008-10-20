#include "PipeListener.h"
#include "IO/Proactor.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"I/O"

using namespace Musador;

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
PipeListener::friendlyName()
{
    std::string r = Util::unicodeToUtf8(this->name);
    size_t pos = r.find_last_of("\\/");
    if (pos == std::string::npos)
    {
        return r;
    } 
    else 
    {
        return r.substr(r.find_last_of("\\/") + 1);
    }
}