#include "bufferChain.h"


// Iterator

BufferChain::BufferChain()
{

}

BufferChain::~BufferChain()
{

}

BufferChain::iterator::iterator()
{

}

BufferChain::iterator::iterator(const iterator& iter)
{

}

BufferChain::iterator& 
BufferChain::iterator::operator=(const BufferChain::iterator& iter)
{

}

BufferChain::iterator::~iterator()
{

}

bool
BufferChain::iterator::operator==(const BufferChain::iterator& iter) const
{

}

bool
BufferChain::iterator::operator!=(const BufferChain::iterator& iter) const
{

}

unsigned char&
BufferChain::iterator::operator*() const
{

}

BufferChain::iterator& 
BufferChain::iterator::operator++()
{

}

BufferChain::iterator& 
BufferChain::iterator::operator++(int)
{

}

BufferChain::iterator& 
BufferChain::iterator::operator--()
{

}

BufferChain::iterator& 
BufferChain::iterator::operator--(int)
{

}

// const_iterator

BufferChain::const_iterator::const_iterator()
{

}

BufferChain::const_iterator::const_iterator(const BufferChain::const_iterator& iter)
{

}

BufferChain::const_iterator::~const_iterator()
{

}

bool
BufferChain::const_iterator::operator==(const BufferChain::const_iterator& iter) const
{

}

bool
BufferChain::iterator::operator!=(const BufferChain::const_iterator& iter) const
{

}

const unsigned char&
BufferChain::const_iterator::operator*() const
{

}

BufferChain::const_iterator& 
BufferChain::const_iterator::operator++()
{

}

BufferChain::const_iterator& 
BufferChain::const_iterator::operator++(int)
{

}

BufferChain::const_iterator& 
BufferChain::const_iterator::operator--()
{

}

BufferChain::const_iterator& 
BufferChain::const_iterator::operator--(int)
{

}