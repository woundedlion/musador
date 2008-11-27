#include <cxxtest\TestSuite.h>

#include "Utilities/BufferChain.h"
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
        LOG(Info) << "Util::BufferChain test end...";
    }

    void setUp() 
    {
    }

    void tearDown()
    {
    }
};