#ifndef LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48
#define LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/lexical_cast.hpp"
#include <sstream>
#include <queue>
#include "Utilities/Util.h"
#include "Utilities/Singleton.h"

#define SET_LOG_SENDER(sender)

namespace Musador
{
	typedef boost::condition Condition;
	typedef boost::thread Thread;
	typedef boost::mutex Mutex;
	typedef boost::mutex::scoped_lock Guard;

	typedef enum 
	{
		Debug = 'D',
		Info = 'I',
		Warning = 'W',
		Error = 'E',
		Critical = 'C'
	} LogLevel;

	class Logger : public Singleton<Logger>
	{
	friend class LogWriter;
	public:
	
		Logger();
		
		~Logger();

		void shutdown();

		void run();

		LogWriter operator()(LogLevel lvl);
		LogWriter operator()(LogLevel lvl, const std::string& sender);
		LogWriter operator()(LogLevel lvl, const std::wstring& sender);
		LogWriter operator()(LogLevel lvl, const std::string& sender, const std::string& tag);
		LogWriter operator()(LogLevel lvl, const std::wstring& sender, const std::wstring& tag);

	private:

		void log(const std::wstring& msg);

		Mutex logLock;
		Condition logPending;	
		std::queue<std::wstring> logMessages;

		Thread * logThread;
	};

	class LogWriter
	{
	public:

		LogWriter(Logger * logger, LogLevel lvl, const std::wstring& sender, const std::wstring& tag);

		~LogWriter();

		template <typename T>
		LogWriter& operator<<(const T& msg);

	private:

		Logger * logger;
		std::wstringstream logStream;
	};

	template <typename T>
	inline LogWriter& LogWriter::operator<<(const T& msg)
	{
		this->logStream << msg;
		return (*this);
	}

	template <>
	inline LogWriter& LogWriter::operator<<<char>(const char& msg)
	{
		this->logStream << Util::utf8ToUnicode(msg);
		return (*this);
	}

	template <>
	inline LogWriter& LogWriter::operator<<<std::string>(const std::string& msg)
	{
		this->logStream << Util::utf8ToUnicode(msg);
		return (*this);
	}
}

#define LOG (*Musador::Logger::instance())

#endif