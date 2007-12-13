#ifndef LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48
#define LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48

#include "boost/thread/mutex.hpp"
#include <string>

namespace Musador
{
	typedef boost::thread::mutex Mutex;
	typedef boost::mutex::scoped_lock Guard;

	typedef enum 
	{
		Debug = 'D',
		Info = 'I',
		Warning = 'W',
		Error = 'E',
		Critical = 'C'
	} LogLevel;

	void

	class Logger
	{

	public:

		Logger();
		~Logger();

		void lock();
		void unlock();
		LogStream& stream();

	private:

		static Mutex streamLock;
		std::ostream * logStream;

	};

	template <typename T> 
	LogStream& operator<<(Logger logger, const T& msg)
	{
		logger.lock();
		return logger.stream();
	}


}


#endif