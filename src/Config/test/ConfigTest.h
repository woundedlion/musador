#ifndef CONFIG_TEST_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_TEST_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include <iostream>
#include <fstream>
#include "boost/archive/xml_woarchive.hpp"
#include "boost/archive/xml_wiarchive.hpp"
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
		std::wofstream ofs(L"ConfigTest.xml");
		boost::archive::xml_woarchive ar(ofs);
		SiteConfig site;
		site.addr = "127.0.0.1";
		site.port = 5152;
		cfg.server.sites.push_back(site);
		ar << boost::serialization::make_nvp("Config",cfg);
		ofs.close();

		std::wifstream ifs(L"ConfigTest.xml");
		ifs.seekg(std::ios::beg);
		while (!ifs.eof())
		{
			std::wstring line;
			getline(ifs,line);
			std::wcout << line << std::endl;
		}

	}
};

#endif