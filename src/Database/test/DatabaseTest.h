#include <cxxtest\TestSuite.h>

#include "Database.h"
#include "Logger/Logger.h"

#define LOG_SENDER L"DatabaseTest"

using namespace Musador;

class DatabaseTest : public CxxTest::TestSuite 
{

public:

	unsigned int entId;
	
	DatabaseTest()
	{
        Logging::Logger::instance();
	}

	~DatabaseTest()
	{
        Logging::Logger::destroy();
	}

	void setUp() 
	{
	}

	void tearDown()
	{
	}


	void testSqliteCreate() 
	{
		boost::shared_ptr<Database::IDatabase> db(new Database::DatabaseSqlite(L"testDB.db"));

		try
		{
			db->execute(L"DROP TABLE IF EXISTS test");
			db->execute(L"CREATE TABLE test ("\
                        L"id INTEGER PRIMARY KEY AUTOINCREMENT,"\
                        L"test_int INTEGER,"\
                        L"test_str TEXT,"\
                        L"test_pchar TEXT,"\
                        L"test_wstr TEXT,"\
                        L"test_pwchar_t TEXT"\
                        L")");
			LOG(Info) << "Successfully created database tables";
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "Unable to initialize database tables: Error " << e.what();
			TS_FAIL(e.what());
		}

		LOG(Info) << "testSqliteCreate Success!";
	}

	class EntityTest : public Database::Entity
	{
	public:
		EntityTest(boost::shared_ptr<Database::IDatabase> db) :
		// bind Database and Table
		Database::Entity(db,L"test"),
		// bind Column objects to physical table columns
		testInt(L"test_int"),
		testStr(L"test_str"),
		testPchar(L"test_pchar"),
		testWStr(L"test_wstr"),
		testPwchar_t(L"test_pwchar_t")
		{
			// Register columns
			reg(&testInt);
			reg(&testStr);
			reg(&testPchar);
			reg(&testWStr);
			reg(&testPwchar_t);
		}
		
		~EntityTest() {}

		Database::Column<int> testInt;
		Database::Column<std::string> testStr;
		Database::Column<char *> testPchar;
		Database::Column<std::wstring> testWStr;
		Database::Column<wchar_t *> testPwchar_t;
	};

	void testSqliteInsert()
	{

		// Open database
		boost::shared_ptr<Database::IDatabase> db(new Database::DatabaseSqlite(L"testDB.db"));
		EntityTest ent(db);

		char charBuf[32];
		wchar_t wCharBuf[32];
		::sprintf(charBuf,"%s","Testing char *");
		::swprintf(wCharBuf,32,L"%s", L"Testing wchar_t *");

		// Set the values and test dirty flag
		TS_ASSERT(false == ent.testInt.isDirty());
		ent.testInt = 1234;
		TS_ASSERT(true == ent.testInt.isDirty());

		TS_ASSERT(false == ent.testStr.isDirty());
		ent.testStr = std::string("Testing std::string");
		TS_ASSERT(true == ent.testStr.isDirty());

		TS_ASSERT(false == ent.testPchar.isDirty());
		ent.testPchar = charBuf;
		TS_ASSERT(true == ent.testPchar.isDirty());

		TS_ASSERT(false == ent.testWStr.isDirty());
		ent.testWStr = std::wstring(L"Testing std::wstring");
		TS_ASSERT(true == ent.testWStr.isDirty());

		TS_ASSERT(false == ent.testPwchar_t.isDirty());
		ent.testPwchar_t = wCharBuf;
		TS_ASSERT(true == ent.testPwchar_t.isDirty());
		
		try
		{
			ent.save();
			this->entId = ent.getId();
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "save() failed: " << e.what();
			TS_FAIL(e.what());
		}

		// Now load the values into a new entity and compare
		EntityTest ent2(db);
		char charBuf2[32];
		wchar_t wCharBuf2[32];
		ent2.testPchar = charBuf2;
		ent2.testPwchar_t = wCharBuf2;

		try
		{
			ent2.load(this->entId);
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "load() failed: " << e.what();
			TS_FAIL(e.what());
		}

		TS_ASSERT(ent2.testInt == 1234);
		TS_ASSERT(ent2.testStr == std::string("Testing std::string"));
		TS_ASSERT(0 == ::strcmp(ent2.testPchar,"Testing char *"));
		TS_ASSERT(ent2.testWStr == std::wstring(L"Testing std::wstring"));
		TS_ASSERT(0 == ::wcscmp(ent2.testPwchar_t,L"Testing wchar_t *"));

		LOG(Info) << "testSqliteInsert Success!";
	}

	void testSqliteUpdate()
	{
		// Open database
		boost::shared_ptr<Database::IDatabase> db(new Database::DatabaseSqlite(L"testDB.db"));
		EntityTest ent(db);

		char charBuf[32];
		wchar_t wCharBuf[32];
		ent.testPchar = charBuf;
		ent.testPwchar_t = wCharBuf;
		
		// Load an existing entity
		try
		{
			ent.load(this->entId);
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "load() failed: " << e.what();
			TS_FAIL(e.what());
		}

		TS_ASSERT(this->entId == ent.getId());

		// Set the values
		::sprintf(charBuf,"%s","Testing2 char *");
		::swprintf(wCharBuf,32,L"%s", L"Testing2 wchar_t *");

		TS_ASSERT(false == ent.testInt.isDirty());
		ent.testInt = 12345;
		TS_ASSERT(true == ent.testInt.isDirty());

		TS_ASSERT(false == ent.testStr.isDirty());
		ent.testStr = std::string("Testing2 std::string");
		TS_ASSERT(true == ent.testStr.isDirty());

		TS_ASSERT(false == ent.testPchar.isDirty());
		ent.testPchar = charBuf;
		TS_ASSERT(true == ent.testPchar.isDirty());

		TS_ASSERT(false == ent.testWStr.isDirty());
		ent.testWStr = std::wstring(L"Testing2 std::wstring");
		TS_ASSERT(true == ent.testWStr.isDirty());

		TS_ASSERT(false == ent.testPwchar_t.isDirty());
		ent.testPwchar_t = wCharBuf;
		TS_ASSERT(true == ent.testPwchar_t.isDirty());
		
		try
		{
			ent.save();
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "save() failed: " << e.what();
			TS_FAIL(e.what());
		}

		// Now load the values into a new entity and compare
		EntityTest ent2(db);
		char charBuf2[32];
		wchar_t wCharBuf2[32];
		ent2.testPchar = charBuf2;
		ent2.testPwchar_t = wCharBuf2;

		try
		{
			ent2.load(ent.getId());
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "load() failed: " << e.what();
			TS_FAIL(e.what());
		}

		TS_ASSERT(ent2.testInt == 12345);
		TS_ASSERT(ent2.testStr == std::string("Testing2 std::string"));
		TS_ASSERT(0 == ::strcmp(ent2.testPchar,"Testing2 char *"));
		TS_ASSERT(ent2.testWStr == std::wstring(L"Testing2 std::wstring"));
		TS_ASSERT(0 == ::wcscmp(ent2.testPwchar_t,L"Testing2 wchar_t *"));

		LOG(Info) << "testSqliteUpdate Success!";
		
	}

	void testSqliteDelete()
	{
		// Open database
		boost::shared_ptr<Database::IDatabase> db(new Database::DatabaseSqlite(L"testDB.db"));
		EntityTest ent(db);

		char charBuf[32];
		wchar_t wCharBuf[32];
		ent.testPchar = charBuf;
		ent.testPwchar_t = wCharBuf;

		// Load an existing entity
		try
		{
			ent.load(this->entId);
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "load() failed: " << e.what();
			TS_FAIL(e.what());
		}

		TS_ASSERT(this->entId == ent.getId());
		
		// Delete it!
		try
		{
			ent.del();
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "del() failed: " << e.what();
			TS_FAIL(e.what());
		}

		// Now a load should fail
		try
		{
			ent.load(this->entId);
		}
		catch (Database::DatabaseException e)
		{
			LOG(Critical) << "load() failed: " << e.what();
			TS_FAIL(e.what());
		}

		LOG(Info) << "testSqliteDelete Success!";

	}

};