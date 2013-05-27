#include <UnitTest++\UnitTest++.h>

#include "IO/BufferChain.h"
#include "Logger/Logger.h"
#define LOG_SENDER L"BufferChainTest"

using namespace Musador;

TEST(BufferChain)
{
	LOG(Info) << "Util::BufferChain test begin...";
	typedef IO::BufferChain<char> BufferChain;
	typedef IO::Buffer<char> Buffer;

	{
		Buffer buf;
		CHECK(buf.empty());
		CHECK(buf.begin() == buf.end());        
	}

	{
		std::string t("abcdef");
		Buffer buf(t.c_str(), t.size());
		CHECK(!buf.empty());
		CHECK(buf.begin() != buf.end());  
		CHECK(std::equal(buf.begin(), buf.end(), t.begin()));
	}

	{
		boost::shared_array<char> a1(new char[32]);
		Buffer b1(a1, 32);
		std::string s1("abcdef");
		std::copy(s1.begin(), s1.end(), a1.get());
		b1.advanceEnd(s1.size());

		boost::shared_array<char> a2(new char[32]);
		Buffer b2(a2, 32);
		std::string s2("ghijklmnop");
		std::copy(s2.begin(), s2.end(), a2.get());
		b2.advanceEnd(s2.size());

		boost::shared_array<char> a3(new char[32]);
		Buffer b3(a3, 32);
		std::string s3("qrs");
		std::copy(s3.begin(), s3.end(), a3.get());
		b3.advanceEnd(s3.size());

		BufferChain chain;
		chain.append(b1);
		chain.append(b2);
		chain.append(b3);

		std::string forward;
		for (BufferChain::iterator iter = chain.begin(); iter != chain.end(); ++iter)
		{
			forward += *iter;
		}
		LOG(Info) << "Forward:" << forward;
		CHECK(forward == "abcdefghijklmnopqrs");

		std::string reverse;
		BufferChain::iterator iter;
		for (iter = --chain.end(); iter != chain.begin(); --iter)
		{
			reverse += *iter;
		}
		reverse += *iter;

		LOG(Info) << "Reverse:" << reverse;
		CHECK(reverse == "srqponmlkjihgfedcba");


		CHECK(std::equal(chain.begin(), chain.end(), "abcdefghijklmnopqrs"));

		BufferChain::iterator beg = chain.begin();
		CHECK(*beg == 'a');

		chain.pop(3);
		CHECK(std::equal(chain.begin(), chain.end(), "defghijklmnopqrs"));
		chain.pop(5);
		CHECK(std::equal(chain.begin(), chain.end(), "ijklmnopqrs"));
		chain.pop(8);
		CHECK(std::equal(chain.begin(), chain.end(), "qrs"));
		chain.pop(3);
		CHECK(chain.empty());
	}

	LOG(Info) << "Util::BufferChain test end...";
}

int main()
{
	Musador::Logging::Logger::instance()->setLevel(Musador::Logging::Info);
	auto err = UnitTest::RunAllTests();
	Musador::Logging::Logger::instance()->destroy();
	return err;
}