#ifndef BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4
#define BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4

#include <iostream>
#include <list>
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"

namespace Musador
{
    namespace IO
    {
        template<typename T>
        class Buffer
        {
        public:

            Buffer();

            Buffer(size_t capacity);

            Buffer(boost::shared_array<T> buf, size_t capacity);

            Buffer(boost::shared_array<T> buf, size_t capacity, size_t dataEnd);

            Buffer(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd);

            ~Buffer();

            size_t numUsed() const;

            size_t numFree() const;

            bool empty() const;

            T * begin() const;

            T * end() const;

            void advanceBegin(size_t num);

            void advanceEnd(size_t num);

            void rewind();

        private:

            boost::shared_array<T> buf;

            size_t capacity;

            size_t endOffset;

            size_t beginOffset;

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

                bidirectional_iterator_base() {};

                virtual ~bidirectional_iterator_base() {};

            };

            class iterator : public bidirectional_iterator_base
            {
            public:

                friend class BufferChain;

                iterator();

                iterator(const iterator& iter);

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

                iterator(const BufferList& bufs, 
                    const typename BufferList::iterator& curBuf, 
                    T * p);

                typename BufferList::value_type& buf();

                const BufferList&  bufs;
                typename BufferList::iterator curBuf;
                T * p;

            };

        public:

            BufferChain();

            ~BufferChain();

            iterator begin();

            iterator end();

            void append(boost::shared_ptr<Buffer<T> > buf);

            void append(boost::shared_array<T> buf, size_t capacity);

            void append(boost::shared_array<T> buf, size_t capacity, size_t dataEnd);

            void append(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd);

        private:

            BufferList data;

        };

        // Buffer

        template<typename T>
        Buffer<T>::Buffer() : 
        buf(NULL),
            capacity(0),
            endOffset(0),
            beginOffset(0)
        {}

        template<typename T>
        Buffer<T>::Buffer(size_t capacity) : 
        buf(new T[capacity]),
            capacity(capacity),
            endOffset(0),
            beginOffset(0)
        {}

        template<typename T>
        Buffer<T>::Buffer(boost::shared_array<T> buf, size_t capacity) : 
        buf(buf),
            capacity(capacity),
            endOffset(capacity),
            beginOffset(0)
        {}

        template<typename T>
        Buffer<T>::Buffer(boost::shared_array<T> buf, size_t capacity, size_t dataEnd) : 
        buf(buf),
            capacity(capacity),
            endOffset(dataEnd),
            beginOffset(0)
        {}

        template<typename T>
        Buffer<T>::Buffer(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd) : 
        buf(buf),
            capacity(capacity),
            endOffset(dataEnd),
            beginOffset(dataBegin)
        {}

        template<typename T>
        Buffer<T>::~Buffer()
        {}

        template<typename T>
        size_t Buffer<T>::numUsed() const
        {
            return this->endOffset - this->beginOffset;
        }

        template<typename T>
        size_t Buffer<T>::numFree() const
        {
            return this->capacity - this->numUsed();
        }

        template<typename T>
        bool Buffer<T>::empty() const
        {
            return this->numUsed() != 0;
        }

        template<typename T>
        T * Buffer<T>::begin() const
        {
            return this->buf.get() + this->beginOffset;
        }

        template<typename T>
        T * Buffer<T>::end() const
        {
            return this->buf.get() + this->endOffset;
        }

        template<typename T>
        void Buffer<T>::advanceBegin(size_t num)
        {
            assert(num <= this->numUsed());
            this->beginOffset += num;
        }

        template<typename T>
        void Buffer<T>::advanceEnd(size_t num)
        {
            assert(num <= this->numFree());
            this->endOffset += num;
        }

        template<typename T>
        void Buffer<T>::rewind()
        {
            this->beginOffset = 0;
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
            for (BufferList::iterator iter = this->data.begin(); iter != this->data.end(); ++iter)
            {
                if (!(*iter)->empty())
                {
                    return BufferChain<T>::iterator(this->data, iter, (*iter)->begin());
                }
            }
            return this->end();
        }

        template<typename T>
        typename BufferChain<T>::iterator BufferChain<T>::end()
        {
            return BufferChain<T>::iterator(this->data, this->data.end(), NULL);
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_ptr<Buffer<T> > buf)
        {
            this->data.push_back(buf);
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t capacity)
        {
            boost::shared_ptr<Buffer<T> > buf(new Buffer<T>(buf, capacity));
            this->data.push_back(buf);
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t capacity, size_t dataEnd)
        {
            boost::shared_ptr<Buffer<T> > buf(new Buffer<T>(buf, capacity, dataEnd));
            this->data.push_back(buf);
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd)
        {
            boost::shared_ptr<Buffer<T> > buf(new Buffer<T>(buf, capacity, dataBegin, dataEnd));
            this->data.push_back(buf);
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

        // Full Constructor (private)
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
#ifdef DEBUG
            bool incrementable = false;
#endif
            // while there is more data in this buffer or another buffer in the chain
            while (this->p != this->buf()->end() || this->buf() != this->bufs.end())
            {
                // increment if there is more data in this buffer
                if (this->p != this->buf()->end())
                {
#ifdef DEBUG
                    this->incrementable = true;
#endif
                    ++this->p;
                    break;
                } 
                else // otherwise try to move to the next buffer
                {
                    ++this->buf();
                    // next buffer in the chain
                    this->p = this->buf()->begin();
                    if (this->p != this->buf()->end())
                    {
#ifdef DEBUG
                        this->incrementable = true;
#endif
                        break;
                    } 
                }
            }

#ifdef DEBUG
            assert(incrementable);
#endif
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
#ifdef DEBUG
                    this->decrementable = true;
#endif
                    --this->p;
                    break;
                } 
                else // otherwise try to move to the previous buffer
                {
                    --this->buf();
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
}


#endif