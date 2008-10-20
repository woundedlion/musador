#ifndef MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26
#define MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26

#include <queue>
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"

namespace Musador
{
    typedef boost::mutex Mutex;
    typedef boost::condition Condition;
    typedef boost::mutex::scoped_lock Guard;

    template <class T>
    class MessageSink
    {
    public:

        MessageSink() {};

        ~MessageSink() {};

        void post(boost::shared_ptr<T> msg);

        boost::shared_ptr<T> getMsg();

        boost::shared_ptr<T> popMsg();

        size_t count();

    private:

        std::queue<boost::shared_ptr<T>> msgs;
        Mutex msgsMutex;
        Condition msgPostedCV;
    };

    template <class T>
    void MessageSink<T>::post(boost::shared_ptr<T> msg)
    {
        Guard guard(this->msgsMutex);
        this->msgs.push(msg);
        this->msgPostedCV.notify_all();
    }

    template <class T>
    boost::shared_ptr<T> MessageSink<T>::getMsg()
    {
        Guard guard(this->msgsMutex);
        while (0 == this->msgs.size())
        {
            this->msgPostedCV.wait(guard);
        }
        return this->msgs.front();
    }

    template <class T>
    boost::shared_ptr<T> MessageSink<T>::popMsg()
    {
        Guard guard(this->msgsMutex);
        while (0 == this->msgs.size())
        {
            this->msgPostedCV.wait(guard);
        }
        boost::shared_ptr<T> r(this->msgs.front());
        this->msgs.pop();
        return r;
    }

    template <class T>
    size_t MessageSink<T>::count()
    {
        Guard guard(this->msgsMutex);
        return this->msgs.size();
    }
}

#endif

