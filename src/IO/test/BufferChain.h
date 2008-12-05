#include <cxxtest\TestSuite.h>

#include "IO/BufferChain.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"BufferChainTest"

using namespace Musador;

class BufferChainTest : public CxxTest::TestSuite 
{

public:

    BufferChainTest()
    {
        Logging::Logger::instance();		
    }

    ~BufferChainTest()
    {
        Logging::Logger::destroy();
    }

    void testBufferChain()
    {
        LOG(Info) << "Util::BufferChain test begin...";
        typedef IO::BufferChain<char> BufferChain;
        BufferChain buf;
        BufferChain::iterator beg = buf.begin();
        BufferChain::iterator end = buf.end();
        LOG(Info) << "Util::BufferChain test end...";
    }

    void setUp() 
    {
    }

    void tearDown()
    {
    }
};