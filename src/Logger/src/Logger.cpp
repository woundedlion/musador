#include "Logger.h"

#include <time.h>
#include <boost/bind.hpp>
#include <iostream>

using namespace Musador;

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

void Logger::shutdown()
{
	this->send(LogStatement(Debug,L"SHUTDOWN"));
	this->logThread->join();
}

void Logger::run()
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

void Logger::setLevel(LogLevel lvl)
{
	this->level = lvl;
}

LogWriter Logger::operator()(LogLevel lvl)
{
	return LogWriter(this,lvl,L"");
}

LogWriter Logger::operator()(LogLevel lvl, const std::string& sender)
{
	return LogWriter(this,lvl,Util::utf8ToUnicode(sender));
}

LogWriter Logger::operator()(LogLevel lvl, const std::wstring& sender)
{
	return LogWriter(this,lvl,sender);
}

void Logger::send(const LogStatement& stmt)
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
	wchar_t timeBuf[20];
	time_t rawTime;
	::time(&rawTime);
	struct tm * locTime = ::localtime(&rawTime);
	::wcsftime(timeBuf,20,L"%y.%m.%d|%H:%M:%S",locTime);
	this->logStream << L"[" << static_cast<char>(lvl & 0xff) << L"] " << timeBuf << L" <" << sender.c_str() << L"> ";
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
ConsoleLogListener::ConsoleLogListener()
#ifdef _WINDOWS
: 
hStd(::GetStdHandle(STD_OUTPUT_HANDLE)),
curLevel(Info)
#endif
{

}
void ConsoleLogListener::send(const LogStatement& stmt)
{
#ifdef _WINDOWS
        if (stmt.lvl != this->curLevel)
        {
            this->curLevel = stmt.lvl;
            WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            switch (stmt.lvl)
            {
                case Debug:
                    color = FOREGROUND_BLUE;
                    break;
                case Warning:
                    color = FOREGROUND_BLUE | FOREGROUND_GREEN;
                    break;
                case Error:
                    color = FOREGROUND_RED | FOREGROUND_GREEN;
                    break;
                case Critical:
                    color = FOREGROUND_RED;
                    break;
            }
            ::SetConsoleTextAttribute(this->hStd, color);
        }
#endif

        std::wcout << stmt.msg << std::endl;
}

//////////////////////////////////////////////////////////////////////////
// Convenience free functions
//////////////////////////////////////////////////////////////////////////
LogWriter Musador::log(LogLevel lvl)
{
	return (*Logger::instance())(lvl);
}

LogWriter Musador::log(LogLevel lvl, const std::string& sender)
{
	return (*Logger::instance())(lvl,sender);
}

LogWriter Musador::log(LogLevel lvl, const std::wstring& sender)
{
	return (*Logger::instance())(lvl,sender);
}


