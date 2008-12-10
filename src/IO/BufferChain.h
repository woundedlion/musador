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

            Buffer(const T * data);

            Buffer(const T * data, size_t capacity);

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

            typedef std::list<Buffer<T> > BufferList;

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

                iterator(const BufferList* bufs, 
                    const typename BufferList::const_iterator& curBuf, 
                    T * p);

                const BufferList* bufs;
                typename BufferList::const_iterator curBuf;
                T * p;
            };

            class const_iterator : public bidirectional_iterator_base
            {
            public:

                friend class BufferChain;

                const_iterator();

                const_iterator(const const_iterator& iter);

                ~const_iterator();

                const_iterator& operator=(const const_iterator& iter);

                bool operator==(const const_iterator& iter) const;

                bool operator!=(const const_iterator& iter) const;

                const T& operator*() const;

                const_iterator& operator++();

                const_iterator operator++(int);

                const_iterator& operator--();

                const_iterator operator--(int);

            private:

                const_iterator(const BufferList * bufs, 
                    const typename BufferList::const_iterator& curBuf, 
                    T * p);

                const BufferList* bufs;
                typename BufferList::const_iterator curBuf;
                const T * p;

            };

        public:

            BufferChain();

            ~BufferChain();

            iterator begin();

            iterator end();

            const_iterator begin() const;

            const_iterator end() const;

            void append(const Buffer<T>& buf);

            void append(boost::shared_array<T> buf, size_t capacity);

            void append(boost::shared_array<T> buf, size_t capacity, size_t dataEnd);

            void append(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd);

            void pop(size_t num);

            bool empty() const;

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
        Buffer<T>::Buffer(const T * data) : 
        buf(new T[::strlen(data)]),
            capacity(::strlen(data)),
            endOffset(capacity),
            beginOffset(0)
        {
            std::copy(data, data + capacity, buf.get());
        }

        template<typename T>
        Buffer<T>::Buffer(const T * data, size_t capacity) : 
        buf(new T[capacity]),
            capacity(capacity),
            endOffset(capacity),
            beginOffset(0)
        {
            std::copy(data, data + capacity, buf.get());
        }

        template<typename T>
        Buffer<T>::Buffer(boost::shared_array<T> buf, size_t capacity) : 
        buf(buf),
            capacity(capacity),
            endOffset(0),
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
            return this->numUsed() == 0;
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
                if (!iter->empty())
                {
                    return BufferChain<T>::iterator(&this->data, iter, iter->begin());
                }
            }
            return this->end();
        }

        template<typename T>
        typename BufferChain<T>::iterator BufferChain<T>::end()
        {
            return BufferChain<T>::iterator(&this->data, this->data.end(), NULL);
        }

        template<typename T>
        typename BufferChain<T>::const_iterator BufferChain<T>::begin() const
        {
            for (BufferList::const_iterator iter = this->data.begin(); iter != this->data.end(); ++iter)
            {
                if (!iter->empty())
                {
                    return BufferChain<T>::const_iterator(&this->data, iter, iter->begin());
                }
            }
            return this->end();
        }

        template<typename T>
        typename BufferChain<T>::const_iterator BufferChain<T>::end() const
        {
            return BufferChain<T>::const_iterator(&this->data, this->data.end(), NULL);
        }

        template<typename T>
        void BufferChain<T>::append(const Buffer<T>& buf)
        {
            this->data.push_back(buf);
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t capacity)
        {
            this->data.push_back(Buffer<T>(buf, capacity));
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t capacity, size_t dataEnd)
        {
            this->data.push_back(Buffer<T>(buf, capacity, 0, dataEnd));
        }

        template<typename T>
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd)
        {
            this->data.push_back(Buffer<T>(buf, capacity, dataBegin, dataEnd));
        }

        template<typename T>
        void BufferChain<T>::pop(size_t num)
        {
            while (num > 0)
            {
                BufferList::iterator buf = this->data.begin();
                assert(buf != this->data.end());
                if (num >= buf->numUsed())
                {
                    num -= buf->numUsed();
                    this->data.pop_front();
                }
                else
                {
                    buf->advanceBegin(num);
                    num = 0;
                }
            }
        }

        template<typename T>
        bool BufferChain<T>::empty() const
        {
            for (BufferList::const_iterator iter = this->data.begin(); iter != this->data.end(); ++iter)
            {
                if (!iter->empty())
                {
                    return false;
                }
            }
            return true; 
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
        BufferChain<T>::iterator::iterator(const BufferList* bufs, typename const BufferList::const_iterator& curBuf, T * p) :
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
            return *this;
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
#ifdef _DEBUG
            // Make sure we are not already the end iterator
            assert(this->curBuf != this->bufs->end() && this->p != NULL);
#endif
            ++this->p;
            if (this->p == this->curBuf->end())
            {
                // Skip to the next buffer that contains data
                do {
                    ++this->curBuf;
                } while (this->curBuf != this->bufs->end() && this->curBuf->empty());

                if (this->curBuf == this->bufs->end())
                {
                    // we are are now the end iterator
                    this->p = NULL;
                }
                else
                {
                    // we found some valid data
                    this->p = this->curBuf->begin();
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
#ifdef _DEBUG
            // Make sure we are not already the begin iterator
            assert(!this->bufs->empty() && (this->curBuf != this->bufs->begin() || this->p != this->curBuf->begin()));
#endif
            if (this->p == NULL || this->p == this->curBuf->begin())
            {
                // Skip to the previous buffer containing data
                do
                {
                    --this->curBuf;
                } while (this->curBuf != this->bufs->begin() && this->curBuf->empty());

#ifdef _DEBUG
                assert(this->curBuf != this->bufs->begin() || !this->curBuf->empty());
#endif                    
                this->p = this->curBuf->end() - 1;
            }
            else
            {
                --this->p;
            }
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

        // const_iterator

        // Default Constructor
        template<typename T>
        BufferChain<T>::const_iterator::const_iterator() :
        bufs(NULL),
            curBuf(NULL),
            p(NULL)
        {}

        // Copy Constructor
        template<typename T>
        BufferChain<T>::const_iterator::const_iterator(const typename BufferChain<T>::const_iterator& c) :
        bufs(c.bufs),
            curBuf(c.curBuf),
            p(c.p)
        {}

        // Full Constructor (private)
        template<typename T>
        BufferChain<T>::const_iterator::const_iterator(const BufferList* bufs, typename const BufferList::const_iterator& curBuf, T * p) :
        bufs(bufs),
            curBuf(curBuf),
            p(p)
        {}


        // Destructor
        template<typename T>
        BufferChain<T>::const_iterator::~const_iterator()
        {}

        // Assignment Operator
        template<typename T>
        typename BufferChain<T>::const_iterator& BufferChain<T>::const_iterator::operator=(typename const BufferChain<T>::const_iterator& c)
        {
            this->bufs = c.bufs;
            this->curBuf = c.curBuf;
            this->p = c.p;
            return *this;
        }

        // Equality Operator
        template<typename T>
        bool BufferChain<T>::const_iterator::operator==(typename const BufferChain<T>::const_iterator& rhs) const
        {
            return this->p == rhs.p;
        }

        // Inequality Operator
        template<typename T>
        bool BufferChain<T>::const_iterator::operator!=(const typename BufferChain<T>::const_iterator& rhs) const
        {
            return this->p != rhs.p;
        }

        // Dereference operator
        template<typename T>
        const T& BufferChain<T>::const_iterator::operator*() const
        {
            return *this->p;
        }

        // Prefix increment operator
        template<typename T>
        typename BufferChain<T>::const_iterator& BufferChain<T>::const_iterator::operator++()
        {
#ifdef _DEBUG
            // Make sure we are not already the end iterator
            assert(this->curBuf != this->bufs->end() && this->p != NULL);
#endif
            ++this->p;
            if (this->p == this->curBuf->end())
            {
                // Skip to the next buffer that contains data
                do {
                    ++this->curBuf;
                } while (this->curBuf != this->bufs->end() && this->curBuf->empty());

                if (this->curBuf == this->bufs->end())
                {
                    // we are are now the end iterator
                    this->p = NULL;
                }
                else
                {
                    // we found some valid data
                    this->p = this->curBuf->begin();
                }
            }

            return *this;
        }

        // Postfix increment operator
        template<typename T>
        typename BufferChain<T>::const_iterator BufferChain<T>::const_iterator::operator++(int)
        {
            typename BufferChain<T>::iterator r(*this);
            ++(*this);
            return r;
        }

        // Prefix decrement operator
        template<typename T>
        typename BufferChain<T>::const_iterator& BufferChain<T>::const_iterator::operator--()
        {
#ifdef _DEBUG
            // Make sure we are not already the begin iterator
            assert(!this->bufs->empty() && (this->curBuf != this->bufs->begin() || this->p != this->curBuf->begin()));
#endif
            if (this->p == NULL || this->p == this->curBuf->begin())
            {
                // Skip to the previous buffer containing data
                do
                {
                    --this->curBuf;
                } while (this->curBuf != this->bufs->begin() && this->curBuf->empty());

#ifdef _DEBUG
                assert(this->curBuf != this->bufs->begin() || !this->curBuf->empty());
#endif                    
                this->p = this->curBuf->end() - 1;
            }
            else
            {
                --this->p;
            }
            return *this;
        }

        // Postfix decrement operator
        template<typename T>
        typename BufferChain<T>::const_iterator BufferChain<T>::const_iterator::operator--(int)
        {
            typename BufferChain<T>::iterator r(*this);
            --(*this);
            return r;
        }
    }
}


#endif