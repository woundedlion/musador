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

		void log(const std::wstring& msg);

		Mutex logLock;
		Condition logPending;	
		std::queue<std::wstring> logMessages;

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