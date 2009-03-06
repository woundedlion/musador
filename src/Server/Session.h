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

    /// @class Session
    /// @brief A collection of key-value pairs associated with an HTTP session
    class Session : public boost::noncopyable
    {
    public:

        /// @brief Constructor
        /// @param[in] key A unique string identifying this Session
        Session(const std::string& key);

        /// @brief Destructor
        virtual ~Session();

        /// @brief Get the key which uniquely identifies this Session
        /// @returns A unique string identifying this Session
        std::string getKey() const;

        /// @brief Get from the Session the value corresponding to the given key
        /// @param[in] key The key for which a value is fetched
        /// @returns The value corresponding to key
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

        /// @brief Set a value in the Session for the given key
        /// @param[in] key The key for which the value is set
        /// @param[in] value The value to set
        template <typename T>
        void set(const std::string& key, T value)
        {
            Guard lock(this->lock);
            this->store[key] = value;
        }

        /// @brief Clear the Session
        /// Empties the Session of all key-value pairs
        void clear();

    protected:

        typedef std::map<std::string,boost::any> StoreType;

        mutable Mutex lock;
        StoreType store;
        std::string key;
    };

}

#endif