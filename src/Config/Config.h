#ifndef CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include "Utilities/Singleton.h"
#include "boost/serialization/nvp.hpp"
#include <boost/serialization/vector.hpp>

namespace Musador
{

	class SiteConfig
	{
	public:
		
		SiteConfig() :
			documentRoot(L"html"),
			addr("0.0.0.0"),
			port(5152),
			requireAuth(false)
		{}
	
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(addr);
			ar & BOOST_SERIALIZATION_NVP(port);
			ar & BOOST_SERIALIZATION_NVP(documentRoot);
			ar & BOOST_SERIALIZATION_NVP(requireAuth);
		}

		std::wstring documentRoot;
		std::string addr;
		unsigned short port;
		bool requireAuth;
	};

	class Controller;

	class ServerConfig
	{
	public:
		
		typedef std::vector<SiteConfig> SiteCollection;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(sites);
		}

		SiteCollection sites;

		/************************/
		Controller * controller;
	};

	class Config : public Singleton<Config>
	{
	public:

		void load(const std::wstring& path);
		void save(const std::wstring& path);

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(server);
		}
		
		ServerConfig server;
	};


}

#endif