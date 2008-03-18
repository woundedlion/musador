#ifndef CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include "Utilities/Singleton.h"
#include "boost/serialization/nvp.hpp"
#include <boost/serialization/vector.hpp>

namespace Musador
{

	class HTTPConfig
	{
	public:
		
		HTTPConfig() :
			documentRoot(L"html"),
			addr("0.0.0.0"),
			port(5152),
			requireAuth(false),
			realm(L"")
		{}
	
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(addr);
			ar & BOOST_SERIALIZATION_NVP(port);
			ar & BOOST_SERIALIZATION_NVP(documentRoot);
			ar & BOOST_SERIALIZATION_NVP(requireAuth);
			ar & BOOST_SERIALIZATION_NVP(realm);
		}

		std::wstring documentRoot;
		std::string addr;
		unsigned short port;
		bool requireAuth;
		std::wstring realm;
	};

	class Controller;

	class ServerConfig
	{
	public:
		
		typedef std::vector<HTTPConfig> HTTPSiteCollection;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(sites);
		}

		HTTPSiteCollection sites;

		/************************/
		Controller * controller;
	};

	class Config : public Singleton<Config>
	{
	public:

		bool load(const std::wstring& path);
		bool save(const std::wstring& path);

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(server);
		}
		
		ServerConfig server;
	};


}

#endif