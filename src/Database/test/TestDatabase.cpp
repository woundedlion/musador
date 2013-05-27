#include "UnitTest++/UnitTest++.h"
#include "Database/Archive.h"
#include "Database/DatabaseSqlite.h"
#include "Logger/Logger.h"
#include "Utilities/Util.h"
#include "boost/filesystem.hpp"
#include "sqlite/sqlite3.h"

#define LOG_SENDER L"TestDatabase"

using namespace storm;
namespace fs = boost::filesystem;

class Fixture
{
public:
	Fixture() :
		db((fs::temp_directory_path() /= "test.db").wstring())
	{
		LOG(Info) << "DB Path: " << db.path();
		sql::wstringstream sql;
		sql << "DROP TABLE IF EXISTS test;"
			"CREATE TABLE test ("
			"id INTEGER PRIMARY KEY"
			",  m1 INTEGER"
			",  m2 TEXT"
			",  m3 TEXT"
			");";

		try {
			sqlite::Transaction txn(db);
			txn.execute(sql.str());
			txn.commit();
		} catch(const std::exception& e) {
			LOG(Error) << e.what();
		}
	}

	sqlite::Database db;
};

TEST_FIXTURE(Fixture, testSqlite)
{
	try {
		CHECK(static_cast<sqlite3 *>(db) != NULL);
		{
			sqlite::Transaction txn(db);
			sql::wstringstream sql;
			sql << "INSERT INTO test "
				"(m1, m2, m3) "
				"VALUES (100, 'm2-1', 'm3-1');";
			auto id1 = txn.insert(sql.str());
			
			sql.str(std::wstring());
			sql << "INSERT INTO test "
				"(m1, m2, m3) "
				"VALUES (200, 'm2-2', 'm3-2');";
			auto id2 = txn.insert(sql.str());
						
			CHECK(id1 < id2);

			sql.str(std::wstring());
			sql << "UPDATE test SET m1=201, m2='abc', m3='def' "
				" WHERE id=" << id2 << ";";
			auto numChanged = txn.update(sql.str());

			CHECK(numChanged == 1);

			txn.commit();
		}

		{
			sqlite::Transaction txn(db);
			sql::wstringstream sql;
			sql << "SELECT * from test ORDER BY id ASC;";
			auto r = txn.select(sql.str());
			auto iter = r.begin();
			CHECK(iter != r.end());
			CHECK(iter->get<int>(1) == 100);
			std::string s = iter->get<std::string>(2);
			CHECK(iter->get<std::string>(2) == "m2-1");
			CHECK(iter->get<std::string>(3) == "m3-1");
			++iter;
			CHECK(iter != r.end());
			CHECK(iter->get<int>(1) == 201);
			CHECK(iter->get<std::string>(2) == "abc");
			CHECK(iter->get<std::string>(3) == "def");
			++iter;
			CHECK(iter == r.end());
		}

		{
			sqlite::Transaction txn(db);
			sql::wstringstream sql;
			sql << "UPDATE test SET m1=0, m2='-', m3='-'; ";
			auto numChanged = txn.update(sql.str());
			CHECK(numChanged == 2);

			sql.str(std::wstring());
			sql << "SELECT * from test ORDER BY id ASC;";
			auto r = txn.select(sql.str());
			auto iter = r.begin();
			CHECK(iter != r.end());
			CHECK(iter->get<int>(1) == 0);
			CHECK(iter->get<std::string>(2) == "-");
			CHECK(iter->get<std::string>(3) == "-");
			++iter;
			CHECK(iter != r.end());
			CHECK(iter->get<int>(1) == 0);
			CHECK(iter->get<std::string>(2) == "-");
			CHECK(iter->get<std::string>(3) == "-");
			++iter;
			CHECK(iter == r.end());

			// Don't commit
		}

		{
			sqlite::Transaction txn(db);
			sql::wstringstream sql;
			sql << "SELECT * from test ORDER BY id ASC;";
			auto r = txn.select(sql.str());
			auto iter = r.begin();
			CHECK(iter != r.end());
			CHECK(iter->get<int>(1) == 100);
			CHECK(iter->get<std::string>(2) == "m2-1");
			CHECK(iter->get<std::string>(3) == "m3-1");
			++iter;
			CHECK(iter != r.end());
			CHECK(iter->get<int>(1) == 201);
			CHECK(iter->get<std::string>(2) == "abc");
			CHECK(iter->get<std::string>(3) == "def");
			++iter;
			CHECK(iter == r.end());
		}

		{
			sqlite::Transaction txn(db);
			sql::wstringstream sql;
			sql << "DELETE FROM test;";
			auto numChanged = txn.update(sql.str());
			CHECK(numChanged == 2);
			txn.commit();
		}

		{
			sqlite::Transaction txn(db);
			auto r = txn.select(L"SELECT COUNT(id) FROM test;");
			auto iter = r.begin();
			CHECK(iter != r.end());
			auto count = iter->get<int>(0);
			CHECK(count == 0);
		}
	} catch(const std::exception& e) {
		LOG(Error) << e.what();
	}
}

class TestEntity
{
public:

	TestEntity() :
		id(0),
		m1(0)
	{}

	template <class Archive>
	void serialize(Archive& ar, unsigned int)
	{
		using namespace boost::serialization;
		ar * BOOST_SERIALIZATION_NVP(id);
		ar & BOOST_SERIALIZATION_NVP(m1);
		ar & BOOST_SERIALIZATION_NVP(m2);
		ar & BOOST_SERIALIZATION_NVP(m3);
	}

	const wchar_t *table() const { return L"test"; }

	int64_t id;
	int m1;
	std::string m2;
	std::wstring m3;
};

TEST_FIXTURE(Fixture, testArchive)
{
	TestEntity entity;
	entity.id=123;
	entity.m1 = 100;
	entity.m2 = "m2m2m2";
	entity.m3 = L"m3m3m3";

	{
		// Update should fail since id does not exist
		try {
			sqlite::Transaction txn(db);
			txn << entity;
			txn.commit();
		} catch(const std::exception e) {			
		}
	}

	{
		sqlite::Transaction txn(db);
		auto r = txn.select(L"SELECT * from test;");
		auto iter = r.begin();
		CHECK(iter == r.end());
	}

	{
		entity.id = 0;
		// Insert should proceed
		sqlite::Transaction txn(db);
		txn << entity;
		txn.commit();
	}

	{
		sqlite::Transaction txn(db);
		auto r = txn.select(L"SELECT * from test;");
		auto iter = r.begin();
		CHECK(iter != r.end());
		if (iter != r.end()) {
			CHECK(iter->get<int>(0) == entity.id);
			CHECK(iter->get<int>(1) == 100);
			CHECK(iter->get<std::string>(2) == "m2m2m2");
			CHECK(iter->get<std::string>(3) == "m3m3m3");
			++iter;
			CHECK(iter == r.end());
		}
	}

	{
		TestEntity entity2;
		entity2.id = entity.id;
		sqlite::Transaction txn(db);
		txn >> entity2;
		CHECK(entity2.id == entity.id);
		CHECK(entity2.m1 == entity.m1);
		CHECK(entity2.m2 == entity.m2);
		CHECK(entity2.m3 == entity.m3);
	}
}

int main()
{
	Musador::Logging::Logger::instance()->setLevel(Musador::Logging::Info);
	auto err = UnitTest::RunAllTests();
	Musador::Logging::Logger::instance()->destroy();
	return err;
}