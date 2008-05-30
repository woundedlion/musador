#include <iostream>
#include <fstream>
#include "boost/archive/xml_woarchive.hpp"
#include "boost/archive/xml_wiarchive.hpp"
#include "boost/archive/archive_exception.hpp"
#include "Config/Config.h"

using namespace Musador;

bool 
Config::load(const std::wstring& path)
{
	std::wifstream ifs(path.c_str());
	try
	{
		boost::archive::xml_wiarchive ar(ifs);
		ar >> boost::serialization::make_nvp("Librarian",*this);
	}
	catch (const boost::archive::archive_exception&)
	{
		return false;		
	}
	return true;
}

bool 
Config::save(const std::wstring& path)
{
	std::wofstream ofs(path.c_str());
	try
	{
		boost::archive::xml_woarchive ar(ofs);
		ar << boost::serialization::make_nvp("Librarian",*this);
	}
	catch (const boost::archive::archive_exception&)
	{
		return false;		
	}
	return true;
}

void
HTTPConfig::addUser(const HTTP::User& u)
{
	this->users[u.getUsername()] = u;
}