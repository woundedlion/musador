#pragma once

#include <vector>
#include <string>
#include <map>

#include "Utilities/Singleton.h"
#include "Server/Server.h"

namespace Musador
{

	class LibraryConfig
	{
	public:

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(id);
			ar & BOOST_SERIALIZATION_NVP(nickname);
			ar & BOOST_SERIALIZATION_NVP(dataFile);
			ar & BOOST_SERIALIZATION_NVP(targets);
		}

		int id;
		std::wstring nickname;
		std::wstring dataFile;
		std::vector<std::wstring> targets;
	};

	class LibrarianConfig
	{
	public:

		typedef std::vector<LibraryConfig> LibraryCollection;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(server);
			ar & BOOST_SERIALIZATION_NVP(dataDir);
			ar & BOOST_SERIALIZATION_NVP(libraries);
		}

		ServerConfig server;
		std::wstring dataDir;
		LibraryCollection libraries;
	};
}