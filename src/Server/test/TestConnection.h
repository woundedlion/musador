#ifndef TEST_CONNECTION_745FB5A8
#define TEST_CONNECTION_745FB5A8

#include "Connection.h"

using namespace Musador;

class TestConnection : public Connection
{
	void accepted() { this->beginRead(); }
	void operator<<(boost::shared_ptr<IOMsgReadComplete> msgRead) { this->beginRead(); }
};

#endif