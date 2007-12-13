#ifndef MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26
#define MESSAGE_SINK_C2446D88_0202_481d_8FCF_49CC1258BB26

#include <queue>
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"

typedef boost::mutex Mutex;
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

	boost::shared_ptr<T> getLastMsg();

	size_t count();

private:

	std::queue<boost::shared_ptr<T>> msgs;
	Mutex msgsMutex;
};

template <class T>
void MessageSink<T>::post(boost::shared_ptr<T> msg)
{
	Guard guard(this->msgsMutex);
	this->msgs.push(msg);
}

template <class T>
boost::shared_ptr<T> MessageSink<T>::getMsg()
{
	Guard guard(this->msgsMutex);
	return this->msgs.front();
}

template <class T>
boost::shared_ptr<T> MessageSink<T>::getLastMsg()
{
	Guard guard(this->msgsMutex);
	return this->msgs.back();
}

template <class T>
boost::shared_ptr<T> MessageSink<T>::popMsg()
{
	Guard guard(this->msgsMutex);
	boost::shared_ptr<T> r = this->msgs.front();
	this->msgs.pop();
	return r;
}

template <class T>
size_t MessageSink<T>::count()
{
	Guard guard(this->msgsMutex);
	return this->msgs.size();
}

#endif

