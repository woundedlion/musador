#pragma once

#include <boost/preprocessor/wstringize.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
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

#define STORM_SERIALIZE_TypedValue(macro, data, elem ) \
	ar & STORM_NVP(elem);

#define STORM_SERIALIZE(...) \
	template <class Archive> \
	void serialize(Archive& ar, unsigned int) \
{ \
	BOOST_PP_SEQ_FOR_EACH(STORM_SERIALIZE_TypedValue, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
}

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