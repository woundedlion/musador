#ifndef BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4
#define BUFFER_CHAIN_H_233EDCAA_C2BE_4032_A964_EFD1BDB1E0B4

#include <iostream>

namespace Util
{

    class BufferChain
    {
    public:

        typedef std::list<boost::shared_ptr<Buffer> > BufferList;

        class Buffer
        {
        public:

            Buffer() : data(NULL), len(0) {}

            unsigned char * buf;

            size_t len;

            size_t off;

        };

        class bidirectional_iterator_base : public std::iterator<
            std::bidirectional_iterator_tag, 
            unsigned char, 
            int, 
            unsigned char *, 
            unsigned char &> 
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

            iterator& operator=(const iterator& iter);

            bool operator==(const iterator& iter) const;

            bool operator!=(const iterator& iter) const;
        
            unsigned char& operator*() const;

            iterator& operator++();

            iterator& operator++(int)'

            iterator& operator--();

            iterator& operator--(int);

        private:

            BufferList::iterator iter;
        };

        class const_iterator : public bidirectional_iterator_base
        {
        public:

            const_iterator();

            iterator();

            iterator(const iterator& iter);

            bool operator==(const iterator& iter) const;

            bool operator!=(const iterator& iter) const;

            const unsigned char& operator*() const;

            const_iterator& operator++();

            const_iterator& operator++(int)'

            const_iterator& operator--();

            const_iterator& operator--(int);

        private:

            BufferList::const_iterator iter;

        };

    public:

        BufferChain();

        ~BufferChain();

        boost::shared_ptr<Buffer> getNewBuffer(size_t len);

        iterator begin();

        iterator end();

        void skip(size_t len);

    private:

        BufferList data;

    };
}


#endif