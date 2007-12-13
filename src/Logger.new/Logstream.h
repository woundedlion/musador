#ifndef LOGSTREAM_EB52F447_182E_48a0_A02E_CFFDD9C96539
#define LOGSTREAM_EB52F447_182E_48a0_A02E_CFFDD9C96539

#include <boost/utility.hpp>

namespace Musador
{
	class LogStream : public boost::noncopyable
	{
	public:
			
		virtual ~LogStream() {};

		virtual LogStream& operator<<(const std::wstring& msg) = 0;
		virtual LogStream& operator<<(const std::string& msg) = 0;
		virtual LogStream& operator<<(const char * msg) = 0;
		virtual LogStream& operator<<(const wchar_t * msg) = 0;
		virtual LogStream& operator<<(unsigned char msg) = 0;
		virtual LogStream& operator<<(char msg) = 0;
		virtual LogStream& operator<<(wchar_t msg) = 0;
		virtual LogStream& operator<<(bool msg) = 0;
		virtual LogStream& operator<<(short msg) = 0;
		virtual LogStream& operator<<(unsigned short msg) = 0;
		virtual LogStream& operator<<(int msg) = 0;
		virtual LogStream& operator<<(unsigned int msg) = 0;
		virtual LogStream& operator<<(long msg) = 0;
		virtual LogStream& operator<<(unsigned long msg) = 0;
		virtual LogStream& operator<<(long long msg) = 0;
		virtual LogStream& operator<<(unsigned long long msg) = 0;
		virtual LogStream& operator<<(float msg) = 0;
		virtual LogStream& operator<<(double msg) = 0;
	};
}

#include "LogStreamConsole.h"
#include "LogStreamNull.h"

#endif