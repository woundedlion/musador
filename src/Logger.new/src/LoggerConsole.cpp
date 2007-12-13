#include <time.h>
#include <boost/lexical_cast.hpp>
#include "Util.h"
#include "LoggerConsole.h"

using namespace Musador;

LoggerConsole::LoggerConsole()
{
	this->logStream = new LogStreamConsole();
}

LoggerConsole::~LoggerConsole()
{
	delete this->logStream;
}

// [LEVEL] yy-mm-dd hh::mm::ss <sender> {tag} msg

LogStream& LoggerConsole::log(LogLevel lvl)
{
	return this->log(lvl, __FILE__, L"");
}

LogStream& LoggerConsole::log(LogLevel lvl, const std::string& sender)
{
	return this->log(lvl, Util::utf8ToUnicode(sender.c_str()), L"");
}

LogStream& LoggerConsole::log(LogLevel lvl, const std::wstring& sender)
{
	return this->log(lvl, sender, L"");
}

LogStream& LoggerConsole::log(LogLevel lvl, const std::string& sender, const std::string& tag)
{
	return this->log(lvl, Util::utf8ToUnicode(sender.c_str()), Util::utf8ToUnicode(tag.c_str()));
}

LogStream& LoggerConsole::log(LogLevel lvl, const std::string& sender, const std::wstring& tag)
{
	return this->log(lvl, Util::utf8ToUnicode(sender.c_str()), tag);
}

LogStream& LoggerConsole::log(LogLevel lvl, const std::wstring& sender, const std::string& tag)
{
	return this->log(lvl, sender, Util::utf8ToUnicode(tag.c_str()));
}

LogStream& LoggerConsole::log(LogLevel lvl, const std::wstring& sender, const std::wstring& tag)
{

	// Timestamp 
	wchar_t timeBuf[20];
	time_t rawTime;
	::time(&rawTime);
	struct tm * locTime = ::localtime(&rawTime);
	::wcsftime(timeBuf,20,L"%y.%m.%d|%H:%M:%S",locTime);

	(*this->logStream) << L'\n' << L"[" << (char)lvl << L"] " << timeBuf << L" <" << sender.c_str() << L">" << L" {" << tag << L"} ";
	return (*this->logStream);
}