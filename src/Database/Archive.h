#pragma once

#include <unordered_map>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include "Database/Sql.h"

#include <boost/lexical_cast.hpp>

namespace storm
{
	struct Attribute
	{
		Attribute() {}
		Attribute(const std::string& name, const std::wstring& value) : 
			name(name),
			value(value)
		{}

		std::string name;
		std::wstring value;
	};

	typedef std::vector<Attribute> Attributes;

	template <typename Database>
	class OArchive
	{
	public:

		typedef boost::mpl::bool_<true> is_saving; 
		typedef boost::mpl::bool_<false> is_loading;

		OArchive(typename Database::Transaction& txn) :
			txn(txn)
		{}

		template <typename Entity>
		OArchive& operator &(Entity& entity)
		{
			boost::serialization::serialize_adl(
				*this, 
				entity,
				boost::serialization::version<Entity>::value);

			if (pkey.value != L"0") {
				update(entity.table());
			} else {
				insert(entity.table());
			}

			return *this;
		}

		template <typename T>
		OArchive& operator *(const boost::serialization::nvp<T>& t)
		{
			pkey = Attribute(t.name(), sql::quote(t.value()));
			setPkey = [=] (typename Database::id_t id) { t.value() = id; };
			return *this;
		}

		template <typename T>
		OArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(Attribute(t.name(), sql::quote(t.value())));
			return *this;
		}

	private:

		void update(const std::wstring& table)
		{
			bool first = true;
			sqlStream << L"UPDATE " << table << L" SET ";
			for (auto attr : attrs) {
				if (!first) {
					sqlStream << L", ";
				} else {
					first = false;
				}
				sqlStream << attr.name << L"=" << attr.value;
			}
			sqlStream << L" WHERE " << pkey.name << L"=" << pkey.value << L";";

			if (txn.update(sqlStream.str()) == 0) {
				throw std::runtime_error((boost::format(
					"Update failed - primary key %1% not found") % 
					Util::unicodeToUtf8(pkey.value)).str());
			}
		}

		void insert(const std::wstring& table)
		{
			bool first = true;
			sqlStream << L"INSERT INTO " << table << L" (";
			for (auto attr : attrs) {				
				if (!first) {
					sqlStream << L", ";
				} else {
					first = false;
				}
				sqlStream << attr.name;
			}
			sqlStream << L") VALUES(";
			first = true;
			for (auto attr : attrs) {				
				if (!first) {
					sqlStream << L", ";
				} else {
					first = false;
				}
				sqlStream << attr.value;
			}
			sqlStream << L");";

			setPkey(txn.insert(sqlStream.str()));
		}

		typename Database::Transaction& txn;
		sql::wstringstream sqlStream;
		Attribute pkey;
		Attributes attrs;
		std::function<void (typename Database::id_t)> setPkey;
	};

	template <typename Database>
	class IArchive
	{
	public:

		typedef boost::mpl::bool_<false> is_saving; 
		typedef boost::mpl::bool_<true> is_loading;

		IArchive(typename Database::Transaction& txn) :
			txn(txn),
			rowIdx(0)
		{}

		template <typename Entity>
		IArchive& operator &(Entity& entity)
		{
			rowIdx = 0;
			boost::serialization::serialize_adl(
				*this, 
				entity,
				boost::serialization::version<Entity>::value);

			load(entity.table());
			return *this;
		}

		template <typename T>
		IArchive& operator *(const boost::serialization::nvp<T>& t)
		{
			pkey = Attribute(t.name(), sql::quote(t.value()));
			return *this;
		}

		template <typename T>
		IArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(t.name());
			loadFuncs.emplace_back([=]() { t.value() = row->get<T>(rowIdx++); });
			return *this;
		}

	private:

		void load(const std::wstring& table)
		{
			sql::wstringstream sql;
			bool first = true;
			sql << "SELECT ";
			for (auto attr : attrs) {
				if (!first) {
					sql << ", ";
				} else {
					first = false;
				}
				sql << attr;
			}
			sql << " FROM " << table << " WHERE " 
				<< pkey.name << "=" << pkey.value << ";";
			auto results = txn.select(sql.str());
			row = results.begin();
			if (row == results.end()) {
				throw std::runtime_error((boost::format("Record not found (%1%=%2%)") 
					% pkey.name % Util::unicodeToUtf8(pkey.value)).str());
			}
			for (auto f : loadFuncs) {
				f();
			}
		}

		typename Database::Transaction& txn;
		Attribute pkey;
		std::vector<std::string> attrs;
		std::vector<std::function<void ()>> loadFuncs;
		typename Database::ResultSet::iterator row;
		uint32_t rowIdx;
	};
}