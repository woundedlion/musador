#ifndef CONFIG_TEST_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_TEST_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include <iostream>
#include <fstream>
#include "boost/archive/xml_woarchive.hpp"
#include "boost/archive/xml_wiarchive.hpp"
#include "boost/lexical_cast.hpp"
#include <cxxtest/TestSuite.h>
#include "Config/Config.h"
#include "Network/Network.h"
#include "Logger/Logger.h"

#define LOG_SENDER L"ConfigTest"
using namespace Musador;

class ConfigTest : public CxxTest::TestSuite
{
public:

	void testConfig()
	{
		Config cfg;

		// populate config object
		cfg.server.sites.clear();
		for (int i = 0; i < 10; i++)
		{
			HTTPConfig site;
			site.addr = "111.222.333.444";
			site.port = 5152 + i;
			site.documentRoot = L"DOC_ROOT_TEST_";
			site.documentRoot += boost::lexical_cast<std::wstring>(i);
			site.requireAuth = (0 == i % 2);
			cfg.server.sites.push_back(site);
		}

		cfg.save(L"ConfigTest.xml");

		// Print config
		std::wifstream ifs(L"ConfigTest.xml");
		ifs.seekg(std::ios::beg);
		while (!ifs.eof())
		{
			std::wstring line;
			getline(ifs,line);
			std::wcout << line << std::endl;
		}
		ifs.close();

		Config cfg2;
		cfg2.load(L"ConfigTest.xml");

		// check the config object
		for (int i = 0; i < 10; i++)
		{
			BOOST_ASSERT(cfg.server.sites[i].addr ==  cfg2.server.sites[i].addr);
			BOOST_ASSERT(cfg.server.sites[i].port ==  cfg2.server.sites[i].port);
			BOOST_ASSERT(cfg.server.sites[i].documentRoot ==  cfg2.server.sites[i].documentRoot);
			BOOST_ASSERT(cfg.server.sites[i].requireAuth ==  cfg2.server.sites[i].requireAuth);
		}
	}
};

#endif