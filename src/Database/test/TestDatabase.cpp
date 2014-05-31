#include <limits>

#include "TestEntities.h"

#include "UnitTest++/UnitTest++.h"
#include "Logger/Logger.h"
#include "boost/filesystem.hpp"

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


TEST_FIXTURE(Fixture, test_sqlite)
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

TEST(test_sqlite_archive)
{
	TestEntityAutoKey entity;
	entity.id=123;
	entity.m1 = 100;
	entity.m2 = "m2m2m2";
	entity.m3 = L"m3m3m3";

	TestEntityCompositeKey eck1;
	eck1.id1 = "bar";
	eck1.id2 = 123;
	eck1.m1 = 100;
	eck1.m2 = "m2m2m2";
	eck1.m3 = L"m3m3m3";

	TestEntityCompositeKey eck2;
	eck2.id1 = "foo";
	eck2.id2 = 123;
	eck2.m1 = 123;
	eck2.m2 = "m2m2m2";
	eck2.m3 = L"m3m3m3";

	sqlite::Database db((fs::temp_directory_path() /= "test.db").wstring());

	try {
		sqlite::Transaction txn(db);
		txn << sql::drop<TestEntityAutoKey>()
			<< sql::drop<TestEntityCompositeKey>()
			<< sql::create<TestEntityAutoKey>()
			<< sql::create<TestEntityCompositeKey>();
		txn.commit();
	}
	catch (const std::exception e) {
		CHECK(false);
	}

	// Update should fail since id does not exist
	try {
		sqlite::Transaction txn(db);
		txn << entity;
		txn.commit();
		CHECK(false);
	} catch(const std::exception e) {			
	}

	// Composite key fails to update since keys don't exist
	try {
		sqlite::Transaction txn(db);
		txn << eck1 << eck2;
		txn.commit();
		CHECK(false);
	}
	catch (const std::exception e) {
	}

	// Check emptiness
	{
		sqlite::Transaction txn(db);
		auto r = txn.select(L"SELECT * from test;");
		auto iter = r.begin();
		CHECK(iter == r.end());
	}

	// Inserts
	{
		entity.id = 0;
		sqlite::Transaction txn(db);
		txn << entity;
		txn.commit();
	}

	{
		sqlite::Transaction txn(db);
		txn << sql::insert(eck1) << sql::insert(eck2);
		txn.commit();
	}

	{
		sqlite::Transaction txn(db);
		auto r = txn.select(L"SELECT * from test;");
		auto iter = r.begin();
		CHECK(iter != r.end());
		if (iter != r.end()) {
			CHECK(iter->get<int>(0) == entity.id);
			CHECK(iter->get<int>(1) == entity.m1);
			CHECK(iter->get<std::string>(2) == entity.m2);
			CHECK(iter->get<std::wstring>(3) == entity.m3);
			++iter;
			CHECK(iter == r.end());
		}
	}

	{
		sqlite::Transaction txn(db);
		auto r = txn.select(L"SELECT * from test_composite_key;");
		auto iter = r.begin();
		CHECK(iter != r.end());
		CHECK(iter->get<std::string>(0) == eck1.id1);
		CHECK(iter->get<int>(1) == eck1.id2);
		CHECK(iter->get<int>(2) == eck1.m1);
		CHECK(iter->get<std::string>(3) == eck1.m2);
		CHECK(iter->get<std::wstring>(4) == eck1.m3);
		++iter;
		CHECK(iter != r.end());
		CHECK(iter->get<std::string>(0) == eck2.id1);
		CHECK(iter->get<int>(1) == eck2.id2);
		CHECK(iter->get<int>(2) == eck2.m1);
		CHECK(iter->get<std::string>(3) == eck2.m2);
		CHECK(iter->get<std::wstring>(4) == eck2.m3);
		++iter;
		CHECK(iter == r.end());
	}


	{
		TestEntityAutoKey entity2;
		entity2.id = entity.id;
		sqlite::Transaction txn(db);
		txn >> entity2;
		CHECK(entity2.id == entity.id);
		CHECK(entity2.m1 == entity.m1);
		CHECK(entity2.m2 == entity.m2);
		CHECK(entity2.m3 == entity.m3);
	}

	{
		TestEntityCompositeKey e;
		e.id1 = eck1.id1;
		e.id2 = eck1.id2;

		sqlite::Transaction txn(db);
		txn >> e;
		CHECK(e.id1 == eck1.id1);
		CHECK(e.id2 == eck1.id2);
		CHECK(e.m1 == eck1.m1);
		CHECK(e.m2 == eck1.m2);
		CHECK(e.m3 == eck1.m3);
	}

	{
		TestEntityCompositeKey e;
		e.id1 = eck2.id1;
		e.id2 = eck2.id2;

		sqlite::Transaction txn(db);
		txn >> e;
		CHECK(e.id1 == eck2.id1);
		CHECK(e.id2 == eck2.id2);
		CHECK(e.m1 == eck2.m1);
		CHECK(e.m2 == eck2.m2);
		CHECK(e.m3 == eck2.m3);
	}
}

TEST(test_json_archive)
{
	TestEntityAutoKey entity;
	entity.id = 123;
	entity.m1 = 100;
	entity.m2 = "m2m2m2";
	entity.m3 = L"m3m3m3";

	TestEntityCompositeKey eck1;
	eck1.id1 = "bar";
	eck1.id2 = 123;
	eck1.m1 = 100;
	eck1.m2 = "m2m2m2";
	eck1.m3 = L"m3m3m3";

	TestNested nested(entity, eck1);

	{
		std::stringstream json;
		storm::json::OutputArchive ar(json);
		ar << entity << eck1;
		ar << nested;
		std::cout << json.str();
	}

	{
		TestReadWrite rw;
		rw.m1 = true;
		rw.m2 = 1;
		rw.m3 = -1;
		rw.m4 = 1;
		rw.m5 = 1;
		rw.m6 = 1;
		rw.m7 = 1;
		rw.m8 = "foo";
		rw.m9 = L"bar";

		std::stringstream json;
		storm::json::OutputArchive out(json);
		out << rw;
		std::cout << json.str();
		TestReadWrite rw2;
		storm::json::InputArchive in(json);
		in >> rw2;
		json.str("");
		json.clear();
		out << rw2;
		std::cout << json.str();
		CHECK(rw == rw2);
	}

	{
		TestReadWrite rw;
		rw.m1 = true;
		rw.m2 = std::numeric_limits<decltype(rw.m2)>::max();
		rw.m3 = std::numeric_limits<decltype(rw.m3)>::max();
		rw.m4 = std::numeric_limits<decltype(rw.m4)>::max();
		rw.m5 = std::numeric_limits<decltype(rw.m5)>::max();
		rw.m6 = std::numeric_limits<decltype(rw.m6)>::max();
		rw.m7 = std::numeric_limits<decltype(rw.m7)>::max();
		rw.m8 = "foo";
		rw.m9 = L"bar";

		std::stringstream json;
		storm::json::OutputArchive out(json);
		out << rw;
		std::cout << json.str();
		TestReadWrite rw2;
		storm::json::InputArchive in(json);
		in >> rw2;
		json.str("");
		json.clear();
		out << rw2;
		std::cout << json.str();
		CHECK(rw == rw2);
	}

	{
		TestReadWrite rw;
		rw.m1 = true;
		rw.m2 = std::numeric_limits<decltype(rw.m2)>::lowest();
		rw.m3 = std::numeric_limits<decltype(rw.m3)>::lowest();
		rw.m4 = std::numeric_limits<decltype(rw.m4)>::lowest();
		rw.m5 = std::numeric_limits<decltype(rw.m5)>::lowest();
		rw.m6 = std::numeric_limits<decltype(rw.m6)>::lowest();
		rw.m7 = std::numeric_limits<decltype(rw.m7)>::lowest();
		rw.m8 = "foo";
		rw.m9 = L"bar";

		std::stringstream json;
		storm::json::OutputArchive out(json);
		out << rw;
		std::cout << json.str();
		TestReadWrite rw2;
		storm::json::InputArchive in(json);
		in >> rw2;
		json.str("");
		json.clear();
		out << rw2;
		std::cout << json.str();
		CHECK(rw == rw2);
	}

	{
		TestReadWrite rw;
		rw.m6 = std::numeric_limits<decltype(rw.m6)>::epsilon();
		rw.m7 = std::numeric_limits<decltype(rw.m7)>::epsilon();

		std::stringstream json;
		storm::json::OutputArchive out(json);
		out << rw;
		std::cout << json.str();
		TestReadWrite rw2;
		storm::json::InputArchive in(json);
		in >> rw2;
		json.str("");
		json.clear();
		out << rw2;
		std::cout << json.str();
		CHECK(rw == rw2);
	}

	{
		TestEntityAutoKey auto_key_ref;
		TestEntityCompositeKey comp_key;
		TestNested nested(auto_key_ref, comp_key);

		std::stringstream json;
		storm::json::OutputArchive out(json);
		out << nested;
		std::cout << json.str();

		TestEntityAutoKey auto_key_ref2;
		TestEntityCompositeKey comp_key2;
		TestNested nested2(auto_key_ref2, comp_key2);

		storm::json::InputArchive in(json);
		in >> nested2;
		json.str("");
		json.clear();
		out << nested2;
		std::cout << json.str();
		CHECK(nested == nested2);
	}

}

int main()
{
	Musador::Logger::instance()->setLevel(Musador::Info);
	auto err = UnitTest::RunAllTests();
	Musador::Logger::instance()->destroy();
	return err;
}