#ifndef LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48
#define LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48

#include <string>
#include "Utilities/Singleton.h"
#include "LogStream.h"

namespace Musador
{
	typedef enum 
	{
		Debug = 'D',
		Info = 'I',
		Warning = 'W',
		Error = 'E',
		Critical = 'C'
	} LogLevel;

	class Logger 
	{

	public:

		virtual LogStream& log(LogLevel lvl) = 0;
		
		virtual LogStream& log(LogLevel lvl, const std::string& sender) = 0;

		virtual LogStream& log(LogLevel lvl, const std::wstring& sender) = 0;
		
		virtual LogStream& log(LogLevel lvl, const std::string& sender, const std::string& tag) = 0;

		virtual LogStream& log(LogLevel lvl, const std::string& sender, const std::wstring& tag) = 0;

		virtual LogStream& log(LogLevel lvl, const std::wstring& sender, const std::string& tag) = 0;

		virtual LogStream& log(LogLevel lvl, const std::wstring& sender, const std::wstring& tag) = 0;

	protected:

		LogStream * logStream;

	};
}

#include "LoggerNull.h"
#include "LoggerConsole.h"

#endif