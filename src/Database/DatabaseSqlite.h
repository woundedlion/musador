#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <functional>
#include <cassert>
#include "boost/iterator/iterator_facade.hpp"
#include "sqlite/sqlite3.h"
#include "Archive.h"

namespace storm {
	namespace sqlite {

		class Transaction;
		class ResultSet;
		
		typedef int64_t id_t;

		class Database
		{
		public:

			typedef id_t id_t;
			typedef Transaction Transaction;
			typedef ResultSet ResultSet;

			Database(const std::wstring& dbName);
			~Database();
			operator sqlite3*() { return db; }
			std::string path() { return dbName; }

		private:

			void open();
			void close();

			std::string dbName;
			sqlite3 *db;
		};

		class Statement
		{
		public:

			Statement(Database& db, const std::wstring& sql);
			operator sqlite3_stmt*() const { return stmt.get(); }

		private:

			friend class ResultSet;

			void prepare(Database& db, const std::wstring& sql);
			void rewind();
			template <typename T>
			void operator()(const T& t);

			std::shared_ptr<sqlite3_stmt> stmt;
		};

		class ResultSet
		{
		public:

			class iterator;

			class Row
			{
			public:

				template <typename T>
				T get(uint32_t col) const;

			private:

				friend class iterator;

				Row();
				Row(const Statement& stmt);

				const Statement *stmt;
			};

			class iterator : public boost::iterator_facade<
				iterator, 
				const Row, 
				boost::forward_traversal_tag>
			{
			public:

				iterator();

			private:

				friend class ResultSet;
				friend class boost::iterator_core_access;

				explicit iterator(Statement *stmt);
				void increment();
				bool equal(const iterator& iter) const;
				const Row& dereference() const;

				Statement *stmt;
				Row row;
			};

			~ResultSet();

			iterator begin();
			iterator end();

		private:

			friend class Transaction;

			ResultSet(const Statement& stmt);

			Statement stmt;
		};

		typedef ResultSet::Row Row;
		class Transaction
		{
		public:
			
			Transaction(Database& db);
			~Transaction();
			void commit();

			void execute(const std::wstring& sql);
			void execute(Statement& sql);
			
			ResultSet select(const std::wstring& sql);
			ResultSet select(Statement& sql);

			size_t update(const std::wstring& sql);
			size_t update(Statement& sql);

			id_t insert(const std::wstring& sql);
			id_t insert(Statement& sql);

			template <typename Entity>
			Transaction& operator<<(Entity& entity)
			{
				OArchive<Database> oa(*this);
				oa & entity;
				return *this;
			}

			template <typename Entity>
			Transaction& operator>>(Entity& entity)
			{
				IArchive<Database> ia(*this);
				ia & entity;
				return *this;
			}

		private:
			
			Database& db;
			bool committed;
		};
	}
}
