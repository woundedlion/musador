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
	this->log(L"SHUTDOWN");
	this->logThread->join();
}

void Logger::run()
{
	while (true)
	{
		Guard guard(this->logLock);
		while (this->logMessages.size() <= 0) this->logPending.wait(guard);
		if (this->logMessages.front() == L"SHUTDOWN")
		{
			std::wcout << L"Logger shutting down..." << std::endl;
			break;
		}
		else
		{
			std::wcout << this->logMessages.front() << std::endl;
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

void Logger::log(const std::wstring& msg)
{
	Guard guard(this->logLock);
	this->logMessages.push(msg);
	this->logPending.notify_one();
}

//////////////////////////////////////////////////////////////////////////
// LogWriter
//////////////////////////////////////////////////////////////////////////
LogWriter::LogWriter(Logger * logger, LogLevel lvl, const std::wstring& sender) :
logger(logger),
active(false),
silent(false)
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
		this->logger->log(this->logStream.str());
	}
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


