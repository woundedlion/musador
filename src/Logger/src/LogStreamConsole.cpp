#include <boost/lexical_cast.hpp>
#include <iostream>
#include <wchar.h>
#include "LogStreamConsole.h"
#include "Utilities/Util.h"

using namespace Musador;


LogStreamConsole::LogStreamConsole()
: ostr(std::cout)
{

}
	
LogStreamConsole::~LogStreamConsole()
{

}

LogStream& LogStreamConsole::operator<<(const std::wstring& msg)
{
	return *this << Util::unicodeToUtf8(msg.c_str());
}

LogStream& LogStreamConsole::operator<<(const std::string& msg)
{
	return *this << msg.c_str();
}

LogStream& LogStreamConsole::operator<<(const char * msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(const wchar_t * msg)
{
	return *this << Util::unicodeToUtf8(msg);
}

LogStream& LogStreamConsole::operator<<(char msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(unsigned char msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(wchar_t msg)
{
	return *this << Util::unicodeToUtf8(msg);
}

LogStream& LogStreamConsole::operator<<(bool msg)
{
	// TODO: i18n
	if (msg)
		std::wcout << L"TRUE";
	else
		std::wcout << L"FALSE";
	return *this;
}

LogStream& LogStreamConsole::operator<<(short msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(unsigned short msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(int msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(unsigned int msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(long msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(unsigned long msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(long long msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(unsigned long long msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(float msg)
{
	std::wcout << msg;
	return *this;
}

LogStream& LogStreamConsole::operator<<(double msg)
{
	std::wcout << msg;
	return *this;
}

