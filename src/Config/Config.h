#ifndef CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define CONFIG_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include "boost/serialization/nvp.hpp"
#include <boost/serialization/vector.hpp>

namespace Musador
{

	class SiteConfig
	{
	public:
		
		SiteConfig() :
		  documentRoot(L"html"),
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

	class ServerConfig
	{
	public:
		
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(sites);
		}

		std::vector<SiteConfig> sites;
	};

	class Config
	{
	public:

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(server);
		}

		ServerConfig server;
	};

}

#endif