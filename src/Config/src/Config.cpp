#include <iostream>
#include <fstream>
#include "boost/archive/xml_woarchive.hpp"
#include "boost/archive/xml_wiarchive.hpp"
#include "Config/Config.h"

using namespace Musador;

void Config::load(const std::wstring& path)
{
	std::wifstream ifs(path.c_str());
	boost::archive::xml_wiarchive ar(ifs);
	ar >> boost::serialization::make_nvp("Librarian",*this);
}

void Config::save(const std::wstring& path)
{
	std::wofstream ofs(path.c_str());
	boost::archive::xml_woarchive ar(ofs);
	ar << boost::serialization::make_nvp("Librarian",*this);
	ofs.close();
}