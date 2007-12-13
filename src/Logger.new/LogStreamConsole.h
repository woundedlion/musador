#ifndef LOGSTREAMCONSOLE_EB52F447_182E_48a0_A02E_CFFDD9C96539
#define LOGSTREAMCONSOLE_EB52F447_182E_48a0_A02E_CFFDD9C96539

#include <string>
#include <iostream>
#include "LogStream.h"

namespace Musador
{
	class LogStreamConsole : public LogStream
	{
	public:
		
		LogStreamConsole();
		
		virtual ~LogStreamConsole();

		LogStream& operator<<(const std::wstring& msg);
		LogStream& operator<<(const std::string& msg);
		LogStream& operator<<(const char * msg);
		LogStream& operator<<(const wchar_t * msg);
		LogStream& operator<<(char msg);
		LogStream& operator<<(unsigned char msg);
		LogStream& operator<<(wchar_t msg);
		LogStream& operator<<(bool msg);
		LogStream& operator<<(short msg);
		LogStream& operator<<(unsigned short msg);
		LogStream& operator<<(int msg);
		LogStream& operator<<(unsigned int msg);
		LogStream& operator<<(long msg);
		LogStream& operator<<(unsigned long msg);
		LogStream& operator<<(long long msg);
		LogStream& operator<<(unsigned long long msg);
		LogStream& operator<<(float msg);
		LogStream& operator<<(double msg);

	private:

		std::ostream & ostr;
	};
}

#endif