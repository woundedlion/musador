#ifndef TIMER_QUEUE_H_876sdfa8
#define TIMER_QUEUE_H_876sdfa8

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/thread.hpp>

#include "Utilities/Singleton.h"

namespace Musador
{

	typedef boost::mutex Mutex;
	typedef boost::mutex::scoped_lock Guard;

	typedef boost::function<void ()> TimerHandler;

	class Timer
	{
		friend class TimerQueue;

	public:

		void cancel();
	
	private:

		Timer(unsigned int intervalMs, TimerHandler handler, bool once, boost::any tag);

		unsigned int timeoutMs;
		TimerHandler handler;
		unsigned int repeatInterval;
		boost::any tag;
	};

	class TimerQueue : public Singleton<TimerQueue>
	{
	public:

		TimerQueue();

		~TimerQueue();

		boost::shared_ptr<Timer> createTimer(unsigned int intervalMs, TimerHandler handler, bool once = true, boost::any tag = NULL);

		void start();

		void stop();

		void run();

	private:

		typedef std::multimap<unsigned int, boost::shared_ptr<Timer> > TimerCollection;
		boost::mutex timersMutex;
		TimerCollection timers;
		std::auto_ptr<boost::thread> timerThread;
		bool doShutdown;
	};

}

#endif