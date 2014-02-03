#pragma once

#include <boost/preprocessor/wstringize.hpp>
#include <boost/serialization/nvp.hpp>

#include "Archive.h"
#include "Json.h"
#include "Sql.h"
#include "DatabaseSqlite.h"

#define STORM_TABLE(name) \
	static const wchar_t *table() { return BOOST_PP_WSTRINGIZE(name); }

#define STORM_NVP BOOST_SERIALIZATION_NVP

#define STORM_AUTO_PKEY_NVP(name) \
	storm::make_auto_pkey(BOOST_SERIALIZATION_NVP(name))

#define STORM_COMP_PKEY_NVP(name) \
	storm::make_composite_pkey(BOOST_SERIALIZATION_NVP(name))

namespace storm {
	
	template <typename T>
	AutoPKey<T>
	make_auto_pkey(const boost::serialization::nvp<T>& name)
	{
		return AutoPKey<T>(name);
	}

	template <typename T>
	CompositePKey<T>
	make_composite_pkey(const boost::serialization::nvp<T>& name)
	{
		return CompositePKey<T>(name);
	}

}