#ifndef PROPERTY_FE6E0F31_B4FB_46d9_9B96_610F1B1EB37D
#define PROPERTY_FE6E0F31_B4FB_46d9_9B96_610F1B1EB37D

#include <boost/thread/mutex.hpp>

namespace Util
{
    typedef boost::mutex Mutex;
    typedef boost::mutex::scoped_lock Guard;

    template <typename T>
    class Property
    {
    public:

        Property();

        Property(const T& data);

        Property(const Property<T>&);

        Property& operator=(const Property<T>& p);

        Property& operator=(const T& data);

        operator T() const;

        bool operator==(const Property<T>& rhs) const;

        bool operator!=(const Property<T>& rhs) const;

        T get() const;

        void set(const T& data);

        /// Serialize this configuration.
        /// @param[in] ar Destination archive for the serialized data.
        /// @param[in] version The version of the archive. Used by boost::serialization version tracking.
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version);

    private:

        mutable Mutex dataMutex;
        T data;

    };

    template <typename T>
    Property<T>::Property() :
    data(T())
    {}

    template <typename T>
    Property<T>::Property(const T& data) :
    data(data)
    {}

    template <typename T>
    Property<T>::Property(const Property<T>& p) :
    data(p.get())
    {}

    template <typename T>
    Property<T>&
        Property<T>::operator=(const Property<T>& p)
    {
        this->set(p.get());
        return *this;
    }

    template <typename T>
    Property<T>&
        Property<T>::operator=(const T& data)
    {
        this->set(data);
        return *this;
    }

    template <typename T>
    Property<T>::operator T() const
    {
        return this->get();
    }

    template <typename T>
    bool
        Property<T>::operator==(const Property<T>& rhs) const
    {
        return this->get() == rhs.get();
    }

    template <typename T>
    bool
        Property<T>::operator!=(const Property<T>& rhs) const
    {
        return this->get() != rhs.get();
    }

    template <typename T>
    T 
        Property<T>::get() const
    {
        Guard lock(this->dataMutex);
        return data;
    }

    template <typename T>
    void
        Property<T>::set(const T& data)
    {
        Guard lock(this->dataMutex);
        this->data = data;
    }

    template <typename T>
    template <class Archive>
    void 
        Property<T>::serialize(Archive & ar, const unsigned int version)
    {
        Guard lock(this->dataMutex);
        ar & BOOST_SERIALIZATION_NVP(data);
    }


}

#endif