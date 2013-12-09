#ifndef BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4
#define BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4

#include <iostream>
#include <list>
#include <bitset>
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"

namespace Musador
{
    namespace IO
    {

        //////////////////////////////////////////////////////////////////////////////////////////////////

        /// @class Buffer
        /// @brief A fixed-size buffer template
        template<typename T>
        class Buffer
        {
        public:

            /// @brief Constructor
            /// Constructs an empty Buffer
            Buffer();

            /// @brief Constructor
            /// Constructs a Buffer of the specified size
            /// @param[in] capacity The total size of the Buffer after construction
            Buffer(size_t capacity);

            /// @brief Constructor
            /// Constructs a Buffer and fills it with data of the specified length. The buffer is sized to match length exactly
            /// @param[in] data The data to place in the Buffer
            /// @param[in] len The length of the data pointed to by data
            Buffer(const T * data, size_t len);

            /// @brief Constructor
            /// Constructs a Buffer and fills it with a shared_array of data of the specified length
            /// @param[in] buf A shared array of data to place in the Buffer
            /// @param[in] len The length of the data pointed to by buf
            Buffer(boost::shared_array<T> buf, size_t len);

            /// @brief Constructor
            /// Constructs a Buffer of a specified capacity and fills it with a shared_array of data of the specified length
            /// @param[in] buf A shared array of data to place in the Buffer
            /// @param[in] capacity The full capacity of the Buffer after construction
            /// @param[in] dataEnd The length of the data pointed to by buf
            Buffer(boost::shared_array<T> buf, size_t capacity, size_t dataEnd);

            /// @brief Constructor
            /// Constructs a Buffer of a specified capacity and fills it from the specified range in a shared_array of data
            /// @param[in] buf A shared array of data to place in the Buffer
            /// @param[in] capacity The full capacity of the Buffer after construction
            /// @param[in] dataEnd The length of the data pointed to by buf
            Buffer(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd);

            /// @brief Destructor
            ~Buffer();

            Buffer<T>& operator=(const Buffer<T>& buf);

            /// @brief Get the length of the readable data in the buffer
            /// @returns The length of the readable data in the buffer
            size_t numUsed() const;

            /// @brief Get the unused capacity of the buffer
            /// @returns The unused capacity of the buffer
            size_t numFree() const;

            /// @brief Check for emptiness
            /// @returns True if the Buffer contains no readable data
            bool empty() const;

            /// @brief Get a pointer to the start of the data in the Buffer
            /// @returns A pointer to the first readable element of the data contained in the Buffer
            T * begin() const;

            /// @brief Get a pointer to the end of the data in the Buffer
            /// @returns A pointer immediately past the end of the last readable element of the data contained in the Buffer
            T * end() const;

            /// @brief Advance the start of the readable data in the Buffer
            /// Essentially, performs a pop by marking the specified number of elements at the start of the Buffer as unreadable.
            /// The newly unused capacity at the head of the Buffer can be reclaimed by calling rewind.
            void advanceBegin(size_t num);

            /// @brief Advance the end of the readbale data in the Buffer
            /// Essentially, performs a push by marking the specified number of elements after the readable data in the Buffer as readable.
            void advanceEnd(size_t num);

            /// @brief Rewind the start of the readable data in the Buffer to the beginning of the entire Buffer
            void rewind();

        private:

            boost::shared_array<T> buf;
            size_t capacity;
            size_t endOffset;
            size_t beginOffset;

        };

        //////////////////////////////////////////////////////////////////////////////////////////////////

        /// @class BufferChain
        /// A chain of Buffers
        template<typename T>
        class BufferChain
        {
        public:

            class iterator;
            class const_iterator;

            typedef std::list<Buffer<T> > BufferList;

            /// @brief Constructor
            BufferChain();

            /// @brief Destructor
            ~BufferChain();

            /// @brief Get an iterator to the start of the readable data in the BufferChain
            /// @returns An iterator to the start of the readable data in the BufferChain
            iterator begin();

            /// @brief Get an iterator past the end of the readable data in the BufferChain
            /// @returns An iterator past the end of the readable data in the BufferChain
            iterator end();

            /// @brief Get a const iterator to the start of the readable data in the BufferChain
            /// @returns An iterator to the start of the readable data in the BufferChain
            const_iterator begin() const;

            /// @brief Get a const iterator past the end of the readable data in the BufferChain
            /// @returns An iterator past the end of the readable data in the BufferChain
            const_iterator end() const;

            /// @brief Append a Buffer to the BufferChain
            /// @param[in] buf The Buffer to append to the chain
            void append(const Buffer<T>& buf);

            /// @brief Append a Buffer constructed from an array of data 
            /// @param[in] buf A shared array of data to append to the chain
            /// @param[in] len The length of the data pointed to by buf
            void append(boost::shared_array<T> buf, size_t len);

            /// @brief Append a Buffer of the specified capacity constructed from an array of data 
            /// @param[in] buf A shared array of data to append to the chain
            /// @param[in] capacity The capacity of the newly appended Buffer
            /// @param[in] dataEnd The length of the data pointed to by buf
            void append(boost::shared_array<T> buf, size_t capacity, size_t dataEnd);

            /// @brief Append a Buffer of the specified capacity constructed from a specified range in an array of data 
            /// @param[in] buf A shared array of data to append to the chain
            /// @param[in] capacity The capacity of the newly appended Buffer
            /// @param[in] dataBegin The position of the start of the data in buf
            /// @param[in] dataEnd The position immediately past the end of the data in buf
            void append(boost::shared_array<T> buf, size_t capacity, size_t dataBegin, size_t dataEnd);

            /// @brief Increment the start of the readable data in the chain by num elements
            /// As elements at the front  of the chain are popped, Buffers are automatically destroyed as they become unused
            /// @param[in] num The number of elements to pop
            void pop(size_t num);

            /// @brief Check the BufferChain for emptiness
            /// @returns True if the BufferChain contains no readable data
            bool empty() const;

            /// @brief Get the length of the readable data in the BufferChain
            /// @returns The total length of all readable data in all Buffers in the chain
            size_t length() const;

            BufferList& buffers();

        private:

            BufferList data;

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

        public:

            /// @class iterator
            /// @brief Bidirectional iterator type for the BufferChain
            /// Allows the BufferChain to be used by algorithms which take Bidirectional Iterators
            class iterator : public bidirectional_iterator_base
            {
                friend class BufferChain;

            public:

                /// @brief Constructor
                /// Default Constructor
                iterator();

                /// @brief Constructor
                /// Copy Constructor
                iterator(const iterator& iter);

                /// @brief Destructor
                ~iterator();

                /// @brief Assignment operator
                /// @param[in] iter The iterator to copy
                /// @returns A reference to itself for operator chaining (e.g. a = b = c)
                iterator& operator=(const iterator& iter);

                /// @brief Equality operator
                /// @param[in] iter The operand to test for equality
                /// @returns True if this iterator is equal to iter
                bool operator==(const iterator& iter) const;

                /// @brief Inequality operator
                /// @param[in] iter The operand to test for inequality
                /// @returns True if this iterator is not equal to iter
                bool operator!=(const iterator& iter) const;

                /// @brief Dereference operator
                /// @returns a reference to the element pointed to by this iterator
                T& operator*() const;

                /// @brief Prefix increment operator
                /// Increment the iterator to point to the next element
                /// @returns a reference to itself after the increment
                iterator& operator++();

                /// @brief Postfix increment operator
                /// Increment the iterator to point to the next element
                /// @returns a copy of itself before the increment
                iterator operator++(int);

                /// @brief Prefix decrement operator
                /// Decrement the iterator to point to the previous element
                /// @returns a reference to itself after the decrement
                iterator& operator--();

                /// @brief Postfix decrement operator
                /// Decrement the iterator to point to the previous element
                /// @returns a copy of itself before the decrement
                iterator operator--(int);

            private:

                iterator(const BufferList* bufs, 
                    const typename BufferList::const_iterator& curBuf, 
                    T * p);

                const BufferList* bufs;
                typename BufferList::const_iterator curBuf;
                T * p;
            };

            /// @class iterator
            /// @brief Bidirectional const iterator type for the BufferChain
            /// Allows the BufferChain to be used by algorithms which take Bidirectional Iterators
            class const_iterator : public bidirectional_iterator_base
            {
                friend class BufferChain;

            public:

                /// @brief Constructor
                /// Default Constructor
                const_iterator();

                /// @brief Constructor
                /// Copy Constructor
                const_iterator(const const_iterator& iter);

                /// @brief Destructor
                ~const_iterator();

                /// @brief Assignment operator
                /// @param[in] iter The iterator to copy
                /// @returns A reference to itself for operator chaining (e.g. a = b = c)
                const_iterator& operator=(const const_iterator& iter);

                /// @brief Equality operator
                /// @param[in] iter The operand to test for equality
                /// @returns True if this iterator is equal to iter
                bool operator==(const const_iterator& iter) const;

                /// @brief Inequality operator
                /// @param[in] iter The operand to test for inequality
                /// @returns True if this iterator is not equal to iter
                bool operator!=(const const_iterator& iter) const;

                /// @brief Dereference operator
                /// @returns a reference to the element pointed to by this iterator
                const T& operator*() const;

                /// @brief Prefix increment operator
                /// Increment the iterator to point to the next element
                /// @returns a reference to itself after the increment
                const_iterator& operator++();

                /// @brief Postfix increment operator
                /// Increment the iterator to point to the next element
                /// @returns a copy of itself before the increment
                const_iterator operator++(int);

                /// @brief Prefix decrement operator
                /// Decrement the iterator to point to the previous element
                /// @returns a reference to itself after the decrement
                const_iterator& operator--();

                /// @brief Postfix decrement operator
                /// Decrement the iterator to point to the previous element
                /// @returns a copy of itself before the decrement
                const_iterator operator--(int);

            private:

                const_iterator(const BufferList * bufs, 
                    const typename BufferList::const_iterator& curBuf, 
                    T * p);

                const BufferList* bufs;
                typename BufferList::const_iterator curBuf;
                const T * p;

            };
        };

        //////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        class BufferStream
        {
        public:
            
            BufferStream();
            ~BufferStream();
            BufferStream& operator<<(const T * data);
            BufferStream& operator>>(T& data);
            bool eof() const;
            bool fail() const;

        private:

            enum Flags
            {
                fail_bit,
                eof_bit,
                NUM_FLAGS,
            };

            size_t nextAllocSize;
            BufferChain<T> chain;
            std::bitset<NUM_FLAGS> flags;
        };

        //////////////////////////////////////////////////////////////////////////
        // Implementations
        //////////////////////////////////////////////////////////////////////////

        // Buffer

        template<typename T>
        Buffer<T>::Buffer() : 
        buf(static_cast<T *>(nullptr)),
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
        Buffer<T>::Buffer(const T * data, size_t len) : 
        buf(new T[len]),
            capacity(len),
            endOffset(len),
            beginOffset(0)
        {
            std::copy(data, data + len, buf.get());
        }

        template<typename T>
        Buffer<T>::Buffer(boost::shared_array<T> buf, size_t len) : 
        buf(buf),
            capacity(len),
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
        Buffer<T>& Buffer<T>::operator=(const Buffer<T>& newBuf)
        {
            this->buf = newBuf.buf;
            this->capacity = newBuf.capacity;
            this->endOffset = newBuf.endOffset;
            this->beginOffset = newBuf.beginOffset;

            return *this;
        }

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
        void BufferChain<T>::append(boost::shared_array<T> buf, size_t len)
        {
            this->data.push_back(Buffer<T>(buf, len));
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

        template<typename T>
        size_t BufferChain<T>::length() const
        {
            size_t len = 0;
            for (BufferList::const_iterator iter = this->data.begin(); iter != this->data.end(); ++iter)
            {
                len += iter->numUsed();
            }
            return len;
        }

        template<typename T>
        typename BufferChain<T>::BufferList& BufferChain<T>::buffers() 
        {
            return this->data;
        }

        // iterator

        // Default Constructor
        template<typename T>
        BufferChain<T>::iterator::iterator() :
        bufs(NULL),
            curBuf(BufferList::const_iterator()),
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
            // Make sure we are not already the end iterator
            assert(this->curBuf != this->bufs->end() && this->p != NULL);
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
            // Make sure we are not already the begin iterator
            assert(!this->bufs->empty() && (this->curBuf != this->bufs->begin() || this->p != this->curBuf->begin()));
            if (this->p == NULL || this->p == this->curBuf->begin())
            {
                // Skip to the previous buffer containing data
                do
                {
                    --this->curBuf;
                } while (this->curBuf != this->bufs->begin() && this->curBuf->empty());
                assert(this->curBuf != this->bufs->begin() || !this->curBuf->empty());
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
            curBuf(BufferList::const_iterator()),
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
            // Make sure we are not already the end iterator
            assert(this->curBuf != this->bufs->end() && this->p != NULL);
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
            // Make sure we are not already the begin iterator
            assert(!this->bufs->empty() && (this->curBuf != this->bufs->begin() || this->p != this->curBuf->begin()));
            if (this->p == NULL || this->p == this->curBuf->begin())
            {
                // Skip to the previous buffer containing data
                do
                {
                    --this->curBuf;
                } while (this->curBuf != this->bufs->begin() && this->curBuf->empty());

                assert(this->curBuf != this->bufs->begin() || !this->curBuf->empty());
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
    
        // BufferStream

        template <typename T>
        BufferStream<T>::BufferStream() :
        nextAllocSize(1024),
        flags(0)
        {

        }

        template <typename T>
        BufferStream<T>::~BufferStream()
        {

        }
            
        template <typename T>
        BufferStream<T>& BufferStream<T>::operator<<(const T * data) 
        {
            while (*data != NULL)
            {
                BufferChain<T>::BufferList& bufs = this->chain.buffers();
                if (bufs.empty() || bufs.back().numFree() == 0)
                { 
                    // No room in the input Buffer, so make a new one and append it to the chain
                    this->chain.append(Buffer<char>(this->nextAllocSize));
                    this->nextAllocSize *= 2;
                }
                
                // Copy as much as we can into the existing Buffer at the tail of chain
                *(bufs.back().end()) = *data++;
                bufs.back().advanceEnd(1);
                this->flags.reset(eof_bit);
            }

            return *this;
        }

        template<typename T>
        BufferStream<T>& BufferStream<T>::operator>>(T&  val)
        {
            if (this->chain.length() == 0) 
            {
                this->flags.set(eof_bit);
            } 
            else
            {
                val = *this->chain.begin();
                this->chain.pop(1);
            }
            return *this;
        }

        template<typename T>
        bool BufferStream<T>::eof() const
        {
            return this->flags.test(eof_bit);
        }

        template<typename T>
        bool BufferStream<T>::fail() const
        {
            return this->flags.test(fail_bit);
        }
    }
}


#endif