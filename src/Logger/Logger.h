#ifndef LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48
#define LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/lexical_cast.hpp"
#include <sstream>
#include <queue>
#include "Utilities/Util.h"
#include "Utilities/Singleton.h"


namespace Musador
{
	typedef boost::condition Condition;
	typedef boost::thread Thread;
	typedef boost::mutex Mutex;
	typedef boost::mutex::scoped_lock Guard;

	typedef enum 
	{
		Debug = 0x0100 | 'D',
		Info = 0x0200 | 'I',
		Warning = 0x0300 | 'W',
		Error = 0x0400 | 'E',
		Critical = 0x0500 | 'C'
	} LogLevel;

        class LogStatement
        {
        public:

            LogStatement(LogLevel lvl, const std::wstring& msg) : lvl(lvl),msg(msg) {}

            LogLevel lvl;
            std::wstring msg;
        };

        class LogListener
        {
        public:

            virtual ~LogListener() {}
            virtual void send(const LogStatement& stmt) = 0;
        };

        class ConsoleLogListener : public LogListener
        {
        public:

            ConsoleLogListener();    
			~ConsoleLogListener();
            void send(const LogStatement& stmt);

        private:

#ifdef _WINDOWS
            HANDLE hOut;        
			WORD oldColor;
            LogLevel curLevel;
			
			static const WORD LOG_COLOR = FOREGROUND_RED | FOREGROUND_GREEN| FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			static const WORD LOG_COLOR_DEBUG = FOREGROUND_BLUE;
			static const WORD LOG_COLOR_INFO = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			static const WORD LOG_COLOR_WARNING = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			static const WORD LOG_COLOR_ERROR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			static const WORD LOG_COLOR_CRITICAL = FOREGROUND_RED | FOREGROUND_INTENSITY;

#endif
        };

	class Logger : public Singleton<Logger>
	{
	friend class LogWriter;
	public:
	
		Logger();
		
		~Logger();

		void shutdown();

		void run();

		void setLevel(LogLevel lvl);

		LogWriter operator()(LogLevel lvl);
		LogWriter operator()(LogLevel lvl, const std::string& sender);
		LogWriter operator()(LogLevel lvl, const std::wstring& sender);

	private:

		void send(const LogStatement& stmt);

		Mutex logLock;
		Condition logPending;	
		std::queue<LogStatement> logMessages;

		LogLevel level;

		Thread * logThread;
	};

	class LogWriter
	{
	public:

		LogWriter(Logger * logger, LogLevel lvl, const std::wstring& sender);

		~LogWriter();

		template <typename T>
		LogWriter& operator<<(const T& msg);

	private:

		Logger * logger;
		std::wstringstream logStream;
		bool active;
		bool silent;
                LogLevel lvl;
	};

	template <typename T>
	inline LogWriter& LogWriter::operator<<(const T& msg)
	{
		if (!this->silent)
		{
			this->active = true;
			this->logStream << msg;
		}
		return (*this);
	}

	template <>
	inline LogWriter& LogWriter::operator<<<char>(const char& msg)
	{
		if (!this->silent)
		{
			this->active = true;
			this->logStream << Util::utf8ToUnicode(msg);
		}
		return (*this);
	}

	template <>
	inline LogWriter& LogWriter::operator<<<std::string>(const std::string& msg)
	{
		if (!this->silent)
		{
			this->active = true;
			this->logStream << Util::utf8ToUnicode(msg);
		}
		return (*this);
	}

	LogWriter log(LogLevel lvl);
	LogWriter log(LogLevel lvl, const std::string& sender);
	LogWriter log(LogLevel lvl, const std::wstring& sender);

	#define LOG(x) log(x,LOG_SENDER)
}


#endif