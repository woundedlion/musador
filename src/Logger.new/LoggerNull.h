#ifndef LOGGER_NULL_F612B2A7_1C81_487f_ABEE_EE60573951DE
#define LOGGER_NULL_F612B2A7_1C81_487f_ABEE_EE60573951DE

#include "Logger.h"

namespace Musador
{
	class LoggerNull :
		public Logger,
		public Singleton<LoggerNull>
	{

		friend class Singleton<LoggerNull>;

	public:

		LogStream & log(LogLevel lvl);

		LogStream & log(LogLevel lvl, const std::string& sender);

		LogStream & log(LogLevel lvl, const std::wstring& sender);
		
		LogStream & log(LogLevel lvl, const std::string& sender, const std::string& tag);

		LogStream & log(LogLevel lvl, const std::string& sender, const std::wstring& tag);

		LogStream & log(LogLevel lvl, const std::wstring& sender, const std::string& tag);

		LogStream & log(LogLevel lvl, const std::wstring& sender, const std::wstring& tag);

	private:

		LoggerNull();

		~LoggerNull();

	};
}

#endif