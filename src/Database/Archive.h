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
	class TableOutputArchive
	{
	public:

		typedef boost::mpl::bool_<true> is_saving; 
		typedef boost::mpl::bool_<false> is_loading;

		TableOutputArchive(typename Database::Transaction& txn) :
			txn(txn)
		{}

		template <typename Entity>
		TableOutputArchive& operator &(Entity& entity)
		{
			boost::serialization::serialize_adl(
				*this, 
				entity,
				boost::serialization::version<Entity>::value);
			create(Entity::table());
			return *this;
		}

		template <typename T>
		TableOutputArchive& operator *(const boost::serialization::nvp<T>& t)
		{
			pkey = Attribute(t.name(), typename Database::sqlType<T>());
			return *this;
		}

		template <typename T>
		TableOutputArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(Attribute(t.name(), typename Database::sqlType<T>()));
			return *this;
		}

	private:

		void create(const std::wstring& table)
		{
			sql::wstringstream sql;
			sql << L"CREATE TABLE IF NOT EXISTS " << table << "(";
			sql << pkey.name << L" " << pkey.value << L" PRIMARY KEY";
			for (auto attr : attrs) {
				sql << L", " << attr.name << L" " << attr.value;
			}
			sql << ");";
			txn.execute(sql.str());
		}

		typename Database::Transaction& txn;
		Attribute pkey;
		Attributes attrs;
	};

	template <typename Database>
	class RowOutputArchive
	{
	public:

		typedef boost::mpl::bool_<true> is_saving; 
		typedef boost::mpl::bool_<false> is_loading;

		RowOutputArchive(typename Database::Transaction& txn) :
			txn(txn)
		{}

		template <typename Entity>
		RowOutputArchive& operator &(Entity& entity)
		{
			boost::serialization::serialize_adl(
				*this, 
				entity,
				boost::serialization::version<Entity>::value);

			if (pkey.value != L"0") {
				update(Entity::table());
			} else {
				insert(Entity::table());
			}

			return *this;
		}

		template <typename T>
		RowOutputArchive& operator *(const boost::serialization::nvp<T>& t)
		{
			pkey = Attribute(t.name(), sql::quote(t.value()));
			setPkey = [=] (typename Database::id_t id) { t.value() = id; };
			return *this;
		}

		template <typename T>
		RowOutputArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(Attribute(t.name(), sql::quote(t.value())));
			return *this;
		}

	private:

		void update(const std::wstring& table)
		{
			sql::wstringstream sql;
			bool first = true;
			sql << L"UPDATE " << table << L" SET ";
			for (auto attr : attrs) {
				if (!first) sql << L", "; else first = false;
				sql << attr.name << L"=" << attr.value;
			}
			sql << L" WHERE " << pkey.name << L"=" << pkey.value << L";";

			if (txn.update(sql.str()) == 0) {
				throw std::runtime_error((boost::format(
					"Update failed - primary key %1% not found") % 
					Util::unicodeToUtf8(pkey.value)).str());
			}
		}

		void insert(const std::wstring& table)
		{
			sql::wstringstream sql;
			bool first = true;
			sql << L"INSERT INTO " << table << L" (";
			for (auto attr : attrs) {
				if (!first) sql << L", "; else first = false;
				sql << attr.name;
			}
			sql << L") VALUES(";
			first = true;
			for (auto attr : attrs) {				
				if (!first) sql << L", "; else first = false;
				sql << attr.value;
			}
			sql << L");";

			setPkey(txn.insert(sql.str()));
		}

		typename Database::Transaction& txn;
		Attribute pkey;
		Attributes attrs;
		std::function<void (typename Database::id_t)> setPkey;
	};

	template <typename Database>
	class RowInputArchive
	{
	public:

		typedef boost::mpl::bool_<false> is_saving; 
		typedef boost::mpl::bool_<true> is_loading;

		RowInputArchive(typename Database::Transaction& txn) :
			txn(txn),
			rowIdx(0)
		{}

		template <typename Entity>
		RowInputArchive& operator &(Entity& entity)
		{
			rowIdx = 0;
			boost::serialization::serialize_adl(
				*this, 
				entity,
				boost::serialization::version<Entity>::value);

			select(Entity::table());
			return *this;
		}

		template <typename T>
		RowInputArchive& operator *(const boost::serialization::nvp<T>& t)
		{
			pkey = Attribute(t.name(), sql::quote(t.value()));
			return *this;
		}

		template <typename T>
		RowInputArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(t.name());
			loadFuncs.emplace_back([=]() { t.value() = row->get<T>(rowIdx++); });
			return *this;
		}

	private:

		void select(const std::wstring& table)
		{
			sql::wstringstream sql;
			bool first = true;
			sql << "SELECT ";
			for (auto attr : attrs) {
				if (!first) sql << L", "; else first = false;
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