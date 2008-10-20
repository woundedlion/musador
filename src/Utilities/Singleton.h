#ifndef SINGLETON_A5964FEA_2879_401b_8BEC_A9FFB5FB2F5E
#define SINGLETON_A5964FEA_2879_401b_8BEC_A9FFB5FB2F5E

#include <boost/utility.hpp>

template <class T>
class Singleton : public boost::noncopyable
{

public:

    static T * instance()
    {
        if (!_instance)
            _instance = new T();
        return _instance;
    }

    static void destroy()
    {
        if (_instance)
        {
            delete _instance;
            _instance = NULL;
        }
    }

protected:

    Singleton() {}

    virtual ~Singleton() {}

private:

    static T * _instance;

};

template <class T> T * Singleton<T>::_instance = NULL;


#endif