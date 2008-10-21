#include "Logger.h"

#include "boost/date_time/posix_time/posix_time.hpp"

#include <boost/bind.hpp>
#include <iostream>

using namespace Musador;
using namespace Musador::Logging;

template LogWriter& LogWriter::operator<<<std::wstring>(const std::wstring&);

//////////////////////////////////////////////////////////////////////////
// Logger
//////////////////////////////////////////////////////////////////////////

Logger::Logger() :
level(Info)
{
    this->logThread = new Thread(boost::bind(&Logger::run,this));
}

Logger::~Logger()
{
    this->shutdown();
    delete this->logThread;
}

void 
Logger::shutdown()
{
    this->send(LogStatement(Debug,L"SHUTDOWN"));
    this->logThread->join();
}

void 
Logger::run()
{
    ConsoleLogListener listener;
    while (true)
    {
        Guard guard(this->logLock);
        while (this->logMessages.size() <= 0) this->logPending.wait(guard);
        if (this->logMessages.front().msg == L"SHUTDOWN")
        {
            listener.send(LogStatement(Info,L"Logger shutting down..."));
            break;
        }
        else
        {
            listener.send(this->logMessages.front());
            this->logMessages.pop();
        }
    }
}

void 
Logger::setLevel(LogLevel lvl)
{
    this->level = lvl;
}

LogWriter 
Logger::operator()(LogLevel lvl)
{
    return LogWriter(this,lvl,L"");
}

LogWriter 
Logger::operator()(LogLevel lvl, const std::string& sender)
{
    return LogWriter(this,lvl,Util::utf8ToUnicode(sender));
}

LogWriter 
Logger::operator()(LogLevel lvl, const std::wstring& sender)
{
    return LogWriter(this,lvl,sender);
}

void 
Logger::send(const LogStatement& stmt)
{
    Guard guard(this->logLock);
    this->logMessages.push(stmt);
    this->logPending.notify_one();
}

//////////////////////////////////////////////////////////////////////////
// LogWriter
//////////////////////////////////////////////////////////////////////////
LogWriter::LogWriter(Logger * logger, LogLevel lvl, const std::wstring& sender) :
logger(logger),
active(false),
silent(false),
lvl(lvl)
{
    if (lvl < logger->level)
    {
        this->silent = true;
        return;
    }

    // Timestamp 
    boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());

    this->logStream << L"[" << static_cast<char>(lvl & 0xff) << L"] " << boost::posix_time::to_simple_wstring(t) << L" <" << sender.c_str() << L"> ";
}

LogWriter::~LogWriter()
{
    if (this->active)
    {
        this->logger->send(LogStatement(this->lvl, this->logStream.str()));
    }
}

//////////////////////////////////////////////////////////////////////////
// LogListener
//////////////////////////////////////////////////////////////////////////
ConsoleLogListener::ConsoleLogListener() :
curLevel(Info)
{
    this->console.setTextColor(LOG_COLOR_INFO);
}

ConsoleLogListener::~ConsoleLogListener()
{
}

void 
ConsoleLogListener::send(const LogStatement& stmt)
{
    if (stmt.lvl != this->curLevel)
    {
        this->curLevel = stmt.lvl;
        UI::Console::TextColor color = UI::Console::COLOR_WHITE_HI;
        switch (stmt.lvl)
        {
        case Debug:
            color = LOG_COLOR_DEBUG;
            break;
        case Info:
            color = LOG_COLOR_INFO;
            break;
        case Warning:
            color = LOG_COLOR_WARNING;
            break;
        case Error:
            color = LOG_COLOR_ERROR;
            break;
        case Critical:
            color = LOG_COLOR_CRITICAL;
            break;
        default:
            return;
        }
        this->console.setTextColor(color);
    }

    std::wcout << stmt.msg << std::endl;
}

//////////////////////////////////////////////////////////////////////////
// Convenience free functions
//////////////////////////////////////////////////////////////////////////
LogWriter 
Logging::log(LogLevel lvl)
{
    return (*Logger::instance())(lvl);
}

LogWriter 
Logging::log(LogLevel lvl, const std::string& sender)
{
    return (*Logger::instance())(lvl,sender);
}

LogWriter 
Logging::log(LogLevel lvl, const std::wstring& sender)
{
    return (*Logger::instance())(lvl,sender);
}


