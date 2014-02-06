#pragma once

#include <vector>
#include <string>
#include <map>

#include "Utilities/Singleton.h"
#include "Server/HTTP.h"

class HTTPConfig
{
public:

	HTTPConfig() :
		documentRoot(L""),
		addr("0.0.0.0"),
		port(5152),
		requireAuth(false),
		realm(L"")
	{}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(documentRoot);
		ar & BOOST_SERIALIZATION_NVP(addr);
		ar & BOOST_SERIALIZATION_NVP(port);
		ar & BOOST_SERIALIZATION_NVP(requireAuth);
		ar & BOOST_SERIALIZATION_NVP(users);
		ar & BOOST_SERIALIZATION_NVP(realm);
	}

	std::wstring documentRoot;
	std::string addr;
	unsigned short port;
	bool requireAuth;
	HTTP::UserCollection users;
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
};

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

	typedef std::map<int, LibraryConfig> LibraryCollection;
	static const int LOCAL_LIB_ID = 0;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(dataDir);
		ar & BOOST_SERIALIZATION_NVP(libraries);
	}

	std::wstring dataDir;
	LibraryCollection libraries;
};

class Config : public Util::Singleton<Config>
{
public:

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(server);
		ar & BOOST_SERIALIZATION_NVP(librarian);
	}

	ServerConfig server;
	LibrarianConfig librarian;

};
