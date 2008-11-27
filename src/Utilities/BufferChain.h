#ifndef BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4
#define BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4

#include <iostream>
#include <list>
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"

namespace Util
{
    template<typename T>
    class Buffer
    {
    public:

        Buffer();

        Buffer(size_t capacity);

        Buffer(const boost::shared_array<T>& buf, size_t capacity);

        ~Buffer();

        size_t numUsed() const;

        size_t numFree() const;

        T * begin();

        T * end();

    private:

        boost::shared_array<T> buf;

        size_t capacity;

        size_t len;

        size_t off;

    };

    template<typename T>
    class BufferChain
    {
    public:

        typedef std::list<boost::shared_ptr<Buffer<T> > > BufferList;

        class bidirectional_iterator_base : public std::iterator<
            std::bidirectional_iterator_tag, 
            T, 
            int, 
            T *, 
            T &> 
        {
        public:

            bidirectional_iterator_base();

            virtual ~bidirectional_iterator_base() = 0;

        };

        class iterator : public bidirectional_iterator_base
        {
        public:

            iterator();

            iterator(const iterator& iter);

            iterator(const BufferList& bufs, 
                const typename BufferList::iterator& curBuf, 
                T * p);

            ~iterator();

            iterator& operator=(const iterator& iter);

            bool operator==(const iterator& iter) const;

            bool operator!=(const iterator& iter) const;

            T& operator*() const;

            iterator& operator++();

            iterator operator++(int);

            iterator& operator--();

            iterator operator--(int);

        private:

            typename BufferList::value_type& buf();

            BufferList bufs;
            typename BufferList::iterator curBuf;
            T * p;

        };

    public:

        BufferChain();

        ~BufferChain();

        iterator begin();

        iterator end();

    private:

        BufferList data;

    };

    // Buffer

    template<typename T>
    Buffer<T>::Buffer() : 
    start(NULL),
        capacity(0),
        len(0),
        off(0)
    {}

    template<typename T>
    Buffer<T>::Buffer(size_t capacity) : 
    start(new T[capacity]),
        capacity(capacity),
        len(0),
        off(0)
    {}

    template<typename T>
    Buffer<T>::Buffer(const boost::shared_array<T>& buf, size_t capacity) : 
    start(buf),
        capacity(capacity),
        len(0),
        off(0)
    {}

    template<typename T>
    Buffer<T>::~Buffer()
    {}

    template<typename T>
    size_t Buffer<T>::numUsed() const
    {
        return this->len - this->off;
    }

    template<typename T>
    size_t Buffer<T>::numFree() const
    {
        return this->capacity - this->numUsed();
    }

    template<typename T>
    T * Buffer<T>::begin()
    {
        return this->buf.get() + this->off;
    }

    template<typename T>
    T * Buffer<T>::end()
    {
        return this->buf.get() + this->len;
    }

    // BufferChain

    template<typename T>
    BufferChain<T>::BufferChain()
    {

    }

    template<typename T>
    BufferChain<T>::~BufferChain()
    {

    }

    template<typename T>
    typename BufferChain<T>::iterator BufferChain<T>::begin()
    {

    }

    template<typename T>
    typename BufferChain<T>::iterator BufferChain<T>::end()
    {

    }

    // iterator

    // Default Constructor
    template<typename T>
    BufferChain<T>::iterator::iterator() :
        bufs(NULL),
        curBuf(NULL),
        p(NULL)
    {}

    // Copy Constructor
    template<typename T>
    BufferChain<T>::iterator::iterator(const typename BufferChain<T>::iterator& c) :
        bufs(c.bufs),
        curBuf(c.curBuf),
        p(c.p)
    {}

    // Full Constructor
    template<typename T>
    BufferChain<T>::iterator::iterator(const BufferList& bufs, typename const BufferList::iterator& curBuf, T * p) :
        bufs(bufs),
        curBuf(curBuf),
        p(p)
    {}


    // Destructor
    template<typename T>
    BufferChain<T>::iterator::~iterator()
    {}

    // Assignment Operator
    template<typename T>
    typename BufferChain<T>::iterator& BufferChain<T>::iterator::operator=(typename const BufferChain<T>::iterator& c)
    {
        this->bufs = c.bufs;
        this->curBuf = c.curBuf;
        this->p = c.p;
    }

    // Equality Operator
    template<typename T>
    bool BufferChain<T>::iterator::operator==(typename const BufferChain<T>::iterator& rhs) const
    {
        return this->p == rhs.p;
    }

    // Inequality Operator
    template<typename T>
    bool BufferChain<T>::iterator::operator!=(const typename BufferChain<T>::iterator& rhs) const
    {
        return this->p != rhs.p;
    }

    // Dereference operator
    template<typename T>
    T& BufferChain<T>::iterator::operator*() const
    {
        return *this->p;
    }

    // Prefix increment operator
    template<typename T>
    typename BufferChain<T>::iterator& BufferChain<T>::iterator::operator++()
    {
        // Make sure the pointer is valid for the current buffer
        assert(this->p >= this->buf()->begin() && this->p <= this->buf->end());

        // increment the pointer if there is more data in the current buffer
        if (this->p != this->buf()->end())
        {
            ++this->p;
        }

        // if we are past-the-end, try to move to the next buffer
        if (this->p == this->buf()->end())
        {
            ++this->buf();
            if (this->buf() != this->bufs.end())
            {
                // next buffer in the chain
                this->p = this->buf()->begin();
            }
        }

        return *this;
    }

    // Postfix increment operator
    template<typename T>
    typename BufferChain<T>::iterator BufferChain<T>::iterator::operator++(int)
    {
        typename BufferChain<T>::iterator r(*this);
        ++(*this);
        return r;
    }

    // Prefix decrement operator
    template<typename T>
    typename BufferChain<T>::iterator& BufferChain<T>::iterator::operator--()
    {
#ifdef DEBUG
        bool decrementable = false;
#endif
        // while there is prev data in this buffer or a previous buffer in the chain
        while (this->p != this->buf()->begin() || this->buf() != this->bufs.begin())
        {
            // decrement if there is prev data in this buffer
            if (this->p != this->buf()->begin())
            {
                --this->p;
#ifdef DEBUG
                this->decrementable = true;
#endif
                break;
            } 
            else // otherwise try to move to the previous buffer
            {
                --this->buf();
                // next buffer in the chain
                this->p = this->buf()->end();
            }
        }

#ifdef DEBUG
        assert(decrementable);
#endif
        return *this;
    }

    // Postfix decrement operator
    template<typename T>
    typename BufferChain<T>::iterator BufferChain<T>::iterator::operator--(int)
    {
        typename BufferChain<T>::iterator r(*this);
        --(*this);
        return r;
    }

    // private functions

    template<typename T>
    typename BufferChain<T>::BufferList::value_type& BufferChain<T>::iterator::buf()
    {
        return (*this->curBuf);
    }

}

#endif