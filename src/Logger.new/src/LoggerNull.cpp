#include "LoggerNull.h"

using namespace Musador;

LoggerNull::LoggerNull()
{
	this->logStream = new LogStreamNull();
}

LoggerNull::~LoggerNull()
{
	delete this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl)
{
	return *this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl, const std::string& sender)
{
	return *this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl, const std::wstring& sender)
{
	return *this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl, const std::string& sender, const std::string& tag)
{
	return *this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl, const std::string& sender, const std::wstring& tag)
{
	return *this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl, const std::wstring& sender, const std::string& tag)
{
	return *this->logStream;
}

LogStream& LoggerNull::log(LogLevel lvl, const std::wstring& sender, const std::wstring& tag)
{
	return *this->logStream;
}