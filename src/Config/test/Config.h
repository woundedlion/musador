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
        ServerConfig::HTTPSiteCollection sites;
        for (int i = 0; i < 10; i++)
        {
            HTTPConfig site;
            site.addr = "111.222.333.444";
            site.port = 5152 + i;
            site.documentRoot = std::wstring(L"DOC_ROOT_TEST_") + boost::lexical_cast<std::wstring>(i);
            site.requireAuth = (0 == i % 2);
            sites.push_back(site);
        }
        cfg.server.sites = sites;
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
        ServerConfig::HTTPSiteCollection sites2 = cfg2.server.sites;
        for (int i = 0; i < 10; i++)
        {
            BOOST_ASSERT(sites[i].addr ==  sites2[i].addr);
            BOOST_ASSERT(sites[i].port ==  sites2[i].port);
            BOOST_ASSERT(sites[i].documentRoot == sites2[i].documentRoot);
            BOOST_ASSERT(sites[i].requireAuth == sites2[i].requireAuth);
        }
    }
};

#endif