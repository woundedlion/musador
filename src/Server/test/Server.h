#ifndef SERVER_TEST_745FB5A8
#define SERVER_TEST_745FB5A8

#include <cxxtest\TestSuite.h>

#include "Server/Server.h"
#include "Utilities/MTRand.h"
#include <string>
#include "boost/thread.hpp"
#include "boost/bind.hpp"
#include "Logger/Logger.h"
#include "Server/EchoListener.h"
#include "Server/EchoConnection.h"

#define LOG_SENDER L"ServerTest"
using namespace Musador;

class ServerTest : public CxxTest::TestSuite 
{

public:

    ServerTest()
    {
        Logging::Logger::instance();
        this->net = Network::instance();
    }

    ~ServerTest()
    {
        Logging::Logger::destroy();
        Network::destroy();
    }

    void setUp() 
    {
        Logging::Logger::instance()->setLevel(Logging::Debug);
        IO::Proactor::instance()->start();
    }

    void tearDown()
    {
        IO::Proactor::instance()->stop();
        IO::Proactor::instance()->destroy();
    }


    volatile long failed;
    volatile long succeeded;
    void testServerIO()
    {
        ServerConfig cfg;
        Server s(cfg);

        s.start();
        s.waitForStart();

        sockaddr_in localEP = {0};
        localEP.sin_family = AF_INET;
        localEP.sin_addr.s_addr = ::inet_addr("0.0.0.0");
        localEP.sin_port = ::htons(5152);
        boost::shared_ptr<IO::Listener> listener(new EchoListener(localEP));
        s.acceptConnections(listener);

        const int BANK_COUNT = 10;
        const int BANK_SIZE = 10;
        const int BANK_DELAY = 100;

        failed = 0;
        succeeded = 0;
        boost::thread * ioThreads[BANK_COUNT * BANK_SIZE];
        for (int i = 0; i < BANK_COUNT; ++i)
        {
            for (int j = 0; j < BANK_SIZE; j++)
            {
                ioThreads[BANK_SIZE*i + j] = new boost::thread(boost::bind(&ServerTest::runServerIO,this));
            }
            ::Sleep(BANK_DELAY);
        }
        for (int i = 0; i < BANK_COUNT * BANK_SIZE; ++i)
        {
            ioThreads[i]->join();
            delete ioThreads[i];
        }


        s.stop();
        s.waitForStop();
        LOG(Info) << succeeded << " connections succeeded.";
        LOG(Info) << failed << " connections failed.";

    }

    /*
    class EchoClient
    {
    public:
    void onRead(boost::shared_ptr<MsgReadComplete> msgRead)
    {

    }

    void onWrite(boost::shared_ptr<MsgWriteComplete> msgWrite)
    {

    }
    };
    */

    void runServerIO()
    {
        SOCKET c = net->socket(AF_INET,SOCK_STREAM,IPPROTO_TCP,false);
        sockaddr_in serverEP = {0};
        serverEP.sin_family = AF_INET;
        serverEP.sin_addr.s_addr = ::inet_addr("127.0.0.1");
        serverEP.sin_port = ::htons(5152);

        try
        {
            net->connect(c,serverEP);
        }
        catch (const NetworkException& e)
        {
            LOG(Error) << e.what();
            net->closeSocket(c);
            ::InterlockedIncrement(&failed);
            return;
        }

        // Fill Data
        MTRand r;
        const int MAX = 4096;
        std::vector<char> data(MAX);
        for (int i = 0; i < MAX; ++i)
        {
            data[i] = static_cast<char>(r.rand(255));
        }

        // Send and receive data in random chunks

        int recvd = 0;
        int sent = 0;
        std::vector<char> echo(MAX);
        while (sent < MAX)
        {
            int nBytes = 1 + static_cast<int>(r.rand(MAX - sent - 1));

            try
            {
                sent += net->send(c, &data[sent], nBytes);
            }
            catch(const NetworkException& e)
            {
                LOG(Error) << e.what();
            }

            while (recvd < sent)
            {
                try
                {
                    recvd += net->recv(c, &echo[recvd], nBytes);
                }
                catch(const NetworkException& e)
                {
                    LOG(Error) << e.what();
                }
            }
            TS_ASSERT(sent == recvd);
        }

        for (int i = 0; i < MAX; ++i)
        {
            TS_ASSERT(data[i] == echo[i]);
        }

        // Clean up
        net->closeSocket(c);
        ::InterlockedIncrement(&succeeded);
    }

private:

    Network * net;
};

#endif