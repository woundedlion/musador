#ifndef LOGGER_PIPE_WIN_F612B2A7_1C81_487f_ABEE_EE60573951DE
#define LOGGER_PIPE_WIN_F612B2A7_1C81_487f_ABEE_EE60573951DE

#include "Logger.h"

namespace Musador
{
	class LoggerPipeWin :
		public Logger,
		public Singleton<LoggerPipeWin>
	{

		friend class Singleton<LoggerPipeWin>;

	public:

		LogStream & log(LogLevel lvl);

		LogStream & log(LogLevel lvl, const std::string& sender);

		LogStream & log(LogLevel lvl, const std::wstring& sender);

		LogStream & log(LogLevel lvl, const std::string& sender, const std::string& tag);

		LogStream & log(LogLevel lvl, const std::string& sender, const std::wstring& tag);

		LogStream & log(LogLevel lvl, const std::wstring& sender, const std::string& tag);

		LogStream & log(LogLevel lvl, const std::wstring& sender, const std::wstring& tag);

	private:

		LoggerPipeWin();

		~LoggerPipeWin();

	};
}

#endif