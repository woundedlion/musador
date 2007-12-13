#ifndef LOG_STREAM_NULL_EB52F447_182E_48a0_A02E_CFFDD9C96539
#define LOG_STREAM_NULL_EB52F447_182E_48a0_A02E_CFFDD9C96539

#include <string>
#include <iostream>
#include "LogStream.h"

namespace Musador
{
	class LogStreamNull : public LogStream
	{
	public:
		
		LogStreamNull() {};
		
		virtual ~LogStreamNull() {};

		LogStream& operator<<(const std::wstring& msg) { return *this; };
		LogStream& operator<<(const std::string& msg) { return *this; };
		LogStream& operator<<(const char * msg) { return *this; };
		LogStream& operator<<(const wchar_t * msg) { return *this; };
		LogStream& operator<<(char msg) { return *this; };
		LogStream& operator<<(unsigned char msg) { return *this; };
		LogStream& operator<<(wchar_t msg) { return *this; };
		LogStream& operator<<(bool msg) { return *this; };
		LogStream& operator<<(short msg) { return *this; };
		LogStream& operator<<(unsigned short msg) { return *this; };
		LogStream& operator<<(int msg) { return *this; };
		LogStream& operator<<(unsigned int msg) { return *this; };
		LogStream& operator<<(long msg) { return *this; };
		LogStream& operator<<(unsigned long msg) { return *this; };
		LogStream& operator<<(long long msg) { return *this; };
		LogStream& operator<<(unsigned long long msg) { return *this; };
		LogStream& operator<<(float msg) { return *this; };
		LogStream& operator<<(double msg) { return *this; };

	private:

	};
}

#endif