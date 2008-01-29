#ifndef NULL_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD
#define NULL_CONNECTION_H_66E5DEFA_BA19_45d1_8990_2F5FBD85F9DD

class NullConnection : public Connection
{
	void accepted() { this->beginRead(); }
	void post(boost::shared_ptr<IOMsgReadComplete> msgRead) { this->beginRead(); }
	void post(boost::shared_ptr<IOMsgWriteComplete> msgRead) { this->beginRead(); }
};

#endif