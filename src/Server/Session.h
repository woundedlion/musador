#ifndef SESSION_H_EC18643C_6239_4528_9ACE_8E6FFB642CEC
#define SESSION_H_EC18643C_6239_4528_9ACE_8E6FFB642CEC

#include <map>
#include <string>
#include "boost/thread/mutex.hpp"
#include "boost/noncopyable.hpp"
#include "boost/any.hpp"

namespace Musador
{

    typedef boost::mutex Mutex;
    typedef boost::mutex::scoped_lock Guard;
    //////////////////////////////////////////////////////////////////////
    /// Session
    //////////////////////////////////////////////////////////////////////

    class Session : public boost::noncopyable
    {
    public:

        Session(const std::string& key);

        virtual ~Session();

        std::string getKey() const;

        template <typename T>
        T get(const std::string& key) const
        {
            Guard lock(this->lock);
            StoreType::const_iterator iter = this->store.find(key);
            if (iter != this->store.end())
            {
                return boost::any_cast<T>(iter->second);
            }
            else
            {
                return T();
            }
        }

        template <typename T>
        void set(const std::string& key, T value)
        {
            Guard lock(this->lock);
            this->store[key] = value;
        }

        void clear();

    protected:

        typedef std::map<std::string,boost::any> StoreType;

        mutable Mutex lock;
        StoreType store;
        std::string key;
    };

}

#endif