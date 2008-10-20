#ifdef WIN32
#include <windows.h>
#endif
#include <vector>
#include <boost/bind.hpp>
#include "TimerQueue.h"

using namespace Musador;

Timer::Timer(unsigned int intervalMs, TimerHandler handler, bool once, boost::any tag) :
timeoutMs(intervalMs),
handler(handler),
repeatInterval(once ? 0 : intervalMs),
tag(tag)
{

}

void
Timer::cancel()
{
    // TODO: implement timer cancelation
}

TimerQueue::TimerQueue() :
doShutdown(false)
{

}

TimerQueue::~TimerQueue()
{
    Guard lock(this->timersMutex);
    this->timers.clear();
}

boost::shared_ptr<Timer> 
TimerQueue::createTimer(unsigned int intervalMs, TimerHandler handler, bool once, boost::any tag)
{
    boost::shared_ptr<Timer> timer(new Timer(intervalMs, handler, once, tag));
    Guard lock(this->timersMutex);
    this->timers.insert(std::make_pair(intervalMs, timer));	
    return timer;
}

void
TimerQueue::run()
{
    TimerCollection::iterator iter;
    while (!this->doShutdown)
    {
        std::vector<TimerCollection::iterator> expired;
        {
            Guard lock(this->timersMutex);
            // Scan the TimerCollection
            for (iter = this->timers.begin(); !this->doShutdown && iter != this->timers.end(); ++iter)
            {
                boost::shared_ptr<Timer> timer = iter->second;
                if (timer->timeoutMs <= 10)
                {
                    // Execute Timer
                    timer->handler();

                    // Expire timer
                    if (0 == timer->repeatInterval)
                    {
                        expired.push_back(iter); // no repeat
                    }
                    else
                    {
                        timer->timeoutMs = timer->repeatInterval; // reset to original interval
                    }
                }
                else
                {
                    // decrement timer
                    timer->timeoutMs -= 10;
                }
            }
            // Erase expired timers
            for (std::vector<TimerCollection::iterator>::iterator iter = expired.begin(); iter != expired.end(); ++iter)
            {
                this->timers.erase(*iter);
            }
        }
        ::Sleep(10);
    }
}

void
TimerQueue::start()
{
    this->timerThread.reset(new boost::thread(boost::bind(&TimerQueue::run,this)));
}

void
TimerQueue::stop()
{
    this->doShutdown = true;
    this->timerThread->join();
}