#pragma once

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

#include <unordered_map>
#include "Database/Sql.h"

#include <boost/lexical_cast.hpp>

namespace storm
{
	template <typename T>
	struct AutoPKey : public boost::serialization::nvp<T>
	{
		AutoPKey(const boost::serialization::nvp<T>& name) : nvp(name) {}
	};

	template <typename T>
	struct CompositePKey : public boost::serialization::nvp<T>
	{
		CompositePKey(const boost::serialization::nvp<T>& name) : nvp(name) {}
	};

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
			pkeys.push_back(Attribute(t.name(), typename Database::sqlType<T>()));
			return *this;
		}

		template <typename T>
		TableOutputArchive& operator +(const boost::serialization::nvp<T>& t)
		{
			pkeys.push_back(Attribute(t.name(), typename Database::sqlType<T>()));
			return *this;
		}

		template <typename T>
		TableOutputArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(Attribute(t.name(), typename Database::sqlType<T>()));
			return *this;
		}

		template <typename T>
		TableOutputArchive& operator &(const AutoPKey<T>& t)
		{
			return (*this) * t;
		}

		template <typename T>
		TableOutputArchive& operator &(const CompositePKey<T>& t)
		{
			return (*this) + t;
		}

	private:

		void create(const std::wstring& table)
		{
			sql::wstringstream sql;
			sql << L"CREATE TABLE IF NOT EXISTS " << table << "(";

			assert(!pkeys.empty());
			bool first = true;
			for (auto pkey : pkeys) {
				if (!first) sql << ", "; else first = false;
				sql << pkey.name << L" " << pkey.value;
			}

			for (auto attr : attrs) {
				sql << L", " << attr.name << L" " << attr.value;
			}

			sql << L", PRIMARY KEY (";
			first = true;
			for (auto pkey : pkeys) {
				if (!first) sql << ", "; else first = false;
				sql << pkey.name;
			}
			sql << "));";

			txn.execute(sql.str());
		}

		typename Database::Transaction& txn;
		Attributes pkeys;
		Attributes attrs;
	};

	template <typename Database>
	class RowOutputArchive
	{
	public:

		typedef boost::mpl::bool_<true> is_saving; 
		typedef boost::mpl::bool_<false> is_loading;

		RowOutputArchive(typename Database::Transaction& txn) :
			txn(txn),
			auto_pkey(false),
			set_pkey([](const typename Database::id_t&) {})
		{}

		template <typename Entity>
		RowOutputArchive& operator &(Entity& entity)
		{
			boost::serialization::serialize_adl(
				*this,
				entity,
				boost::serialization::version<Entity>::value);
			
			if (auto_pkey) {
				assert(pkeys.size() == 1);
				if (pkeys[0].value == L"0") {
					insert(Entity::table());
					return *this;
				}
			}
			update(Entity::table());
			return *this;
		}

		template <typename Entity>
		RowOutputArchive& operator &(sql::explicit_insert<Entity>& entity)
		{
			boost::serialization::serialize_adl(
				*this, 
				entity.e,
				boost::serialization::version<Entity>::value);
			
			insert(Entity::table());
			return *this;
		}

		RowOutputArchive& operator *(const boost::serialization::nvp<typename Database::id_t>& t)
		{
			if (auto_pkey || !pkeys.empty()) {
				throw std::runtime_error("Use of autoincrement requires a single primary key");
			}
			auto_pkey = true;
			pkeys.push_back(Attribute(t.name(), sql::quote(t.value())));
			set_pkey = [=](const typename Database::id_t& id) { t.value() = id; };
			return *this;
		}

		template <typename T>
		RowOutputArchive& operator +(const boost::serialization::nvp<T>& t)
		{
			if (auto_pkey) {
				throw std::runtime_error("Cannot declare both autincrement and composite key");
			}
			pkeys.push_back(Attribute(t.name(), sql::quote(t.value())));
			return *this;
		}

		template <typename T>
		RowOutputArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(Attribute(t.name(), sql::quote(t.value())));
			return *this;
		}

		template <typename T>
		RowOutputArchive& operator &(const AutoPKey<T>& t)
		{
			return (*this) * t;
		}

		template <typename T>
		RowOutputArchive& operator &(const CompositePKey<T>& t)
		{
			return (*this) + t;
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
			sql << L" WHERE ";
			first = true;
			for (auto pkey : pkeys) {
				if (!first) sql << " AND "; else first = false;
				sql << pkey.name << L"=" << pkey.value;
			} 
			sql << ";";

			if (txn.update(sql.str()) == 0) {
				throw std::runtime_error((boost::format(
					"Update failed - record not found: %1%") % 
					Util::unicodeToUtf8(sql.str())).str());
			}
		}

		void insert(const std::wstring& table)
		{
			sql::wstringstream sql;
			bool first = true;
			sql << L"INSERT INTO " << table << L" (";
			if (!auto_pkey) {
				for (auto pkey : pkeys) {
					if (!first) sql << L", "; else first = false;
					sql << pkey.name;
				}
			}
			for (auto attr : attrs) {
				if (!first) sql << L", "; else first = false;
				sql << attr.name;
			}
			sql << L") VALUES(";
			first = true;
			if (!auto_pkey) {
				for (auto pkey : pkeys) {
					if (!first) sql << L", "; else first = false;
					sql << pkey.value;
				}
			}
			for (auto attr : attrs) {
				if (!first) sql << L", "; else first = false;
				sql << attr.value;
			}
			sql << L");";

			set_pkey(txn.insert(sql.str()));
		}

		typename Database::Transaction& txn;
		bool auto_pkey;
		Attributes pkeys;
		Attributes attrs;
		std::function<void (const typename Database::id_t)> set_pkey;
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
			pkeys.push_back(Attribute(t.name(), sql::quote(t.value())));
			return *this;
		}

		template <typename T>
		RowInputArchive& operator +(const boost::serialization::nvp<T>& t)
		{
			pkeys.push_back(Attribute(t.name(), sql::quote(t.value())));
			return *this;
		}

		template <typename T>
		RowInputArchive& operator &(const boost::serialization::nvp<T>& t)
		{
			attrs.emplace_back(t.name());
			loadFuncs.emplace_back([=]() { t.value() = row->get<T>(rowIdx++); });
			return *this;
		}

		template <typename T>
		RowInputArchive& operator &(const AutoPKey<T>& t)
		{
			return (*this) * t;
		}

		template <typename T>
		RowInputArchive& operator &(const CompositePKey<T>& t)
		{
			return (*this) + t;
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
			sql << " FROM " << table << " WHERE ";
			first = true;
			for (auto pkey : pkeys) {
				if (!first) sql << " AND "; else first = false;
				sql << pkey.name << L"=" << pkey.value;
			}
			sql << ";";

			auto results = txn.select(sql.str());
			row = results.begin();
			if (row == results.end()) {
				throw std::runtime_error((boost::format("Select failed: record not found: %1%") 
					% Util::unicodeToUtf8(sql.str())).str());
			}
			for (auto f : loadFuncs) {
				f();
			}
		}

		typename Database::Transaction& txn;
		Attributes pkeys;
		std::vector<std::string> attrs;
		std::vector<std::function<void ()>> loadFuncs;
		typename Database::ResultSet::iterator row;
		uint32_t rowIdx;
	};
}