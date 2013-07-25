#ifndef SERVER_TEST_745FB5A8
#define SERVER_TEST_745FB5A8

#include <UnitTest++/UnitTest++.h>

#include "Server/Server.h"
#include "Utilities/MTRand.h"
#include <string>
#include <thread>
#include <functional>
#include "Logger/Logger.h"
#include "Server/EchoListener.h"
#include "Server/EchoConnection.h"

#define LOG_SENDER L"ServerTest"
using namespace Musador;

namespace {
	long failed;
	long succeeded;
}

void runServerIO()
{
	SOCKET c = Network::instance()->socket(AF_INET,SOCK_STREAM,IPPROTO_TCP,false);
	sockaddr_in serverEP = {0};
	serverEP.sin_family = AF_INET;
	serverEP.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	serverEP.sin_port = ::htons(65534);

	try {
		Network::instance()->connect(c,serverEP);
	} catch (const NetworkException& e) {
		LOG(Error) << e.what();
		Network::instance()->closeSocket(c);
		::InterlockedIncrement(&failed);
		return;
	}

	// Fill Data
	MTRand r;
	const int MAX = 1 >> 16;
	std::vector<char> data(MAX);
	for (int i = 0; i < MAX; ++i) {
		data[i] = static_cast<char>(r.rand(255));
	}

	// Send and receive data in random chunks
	int recvd = 0;
	int sent = 0;
	std::vector<char> echo(MAX);
	while (sent < MAX) {
		int nBytes = 1 + static_cast<int>(r.rand(MAX - sent - 1));

		try {
			sent += Network::instance()->send(c, &data[sent], nBytes);
		} catch(const NetworkException& e) {
			LOG(Error) << e.what();
		}

		while (recvd < sent) {
			try {
				recvd += Network::instance()->recv(c, &echo[recvd], nBytes);
			} catch(const NetworkException& e) {
				LOG(Error) << e.what();
			}
		}
		CHECK(sent == recvd);
	}

	for (int i = 0; i < MAX; ++i) {
		CHECK(data[i] == echo[i]);
	}

	Network::instance()->closeSocket(c);
	::InterlockedIncrement(&succeeded);
}

TEST(ServerIO)
{
	ServerConfig cfg;
	Server s(cfg);

	s.start();
	s.waitForStart();

	sockaddr_in localEP = {0};
	localEP.sin_family = AF_INET;
	localEP.sin_addr.s_addr = ::inet_addr("0.0.0.0");
	localEP.sin_port = ::htons(65534);
	boost::shared_ptr<IO::Listener> listener(new EchoListener(localEP));
	s.acceptConnections(listener);

	const int BANK_COUNT = 100;
	const int BANK_SIZE = 10;
	const int BANK_DELAY = 10;

	failed = 0;
	succeeded = 0;
	std::thread ioThreads[BANK_COUNT * BANK_SIZE];
	for (int i = 0; i < BANK_COUNT; ++i) {
		for (int j = 0; j < BANK_SIZE; j++) {
			ioThreads[BANK_SIZE * i + j] = std::thread(std::bind(&runServerIO));
		}
		::Sleep(BANK_DELAY);
	}
	for (int i = 0; i < BANK_COUNT * BANK_SIZE; ++i) {
		ioThreads[i].join();
	}

	s.stop();
	s.waitForStop();
	LOG(Info) << succeeded << " connections succeeded.";
	LOG(Info) << failed << " connections failed.";
}

int main()
{
	Logger::instance()->setLevel(Musador::Info);
	IO::Proactor::instance()->start();
	auto err = UnitTest::RunAllTests();
	IO::Proactor::instance()->stop();
	IO::Proactor::instance()->destroy();
	Logger::instance()->destroy();
	return err;
}

#endif