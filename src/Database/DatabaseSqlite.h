#ifndef DATABASESQLITE_5C44681D_25C3_4fce_BF1B_B491A877487D
#define DATABASESQLITE_5C44681D_25C3_4fce_BF1B_B491A877487D

#include <string>
#include <map>
#include <boost/thread/tss.hpp>
#include "sqlite/sqlite3.h"
#include "Database.h"
#include "ResultSetSqlite.h"

class DatabaseSqlite : public Database
{
friend class ResultSetSqlite;
public:
	
	DatabaseSqlite(std::wstring databaseName);
	~DatabaseSqlite();

	bool txnBegin();
	bool txnRollback();
	bool txnCommit();

	std::auto_ptr<ResultSet> select(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions);	
	unsigned long insert(const std::wstring& table, const std::vector<ColumnBase *>& columns);	
	bool update(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions );	
	bool remove(const std::wstring& table, const std::wstring& conditions);
	bool execute(const std::wstring& q);

private:

	std::string dbName;
	boost::thread_specific_ptr<sqlite3> db;

	static void tssCleanup(sqlite3 * dbPtr);
	static void err(int errCode, const char * errMsg = NULL);

	bool open();
	bool close();
};

namespace sqlite {

	static std::map<int,DatabaseException * > errors;

	class ErrUnknown : public DatabaseException
	{
	public:
		ErrUnknown() : DatabaseException("Unknown database error") {}
	};

	class ErrSQL : public DatabaseException
	{
	public:
		ErrSQL(const char * errMsg) : DatabaseException(errMsg ? errMsg : "SQL error or missing database" ) {}
	};

	class ErrInternal : public DatabaseException
	{
	public:
		ErrInternal() : DatabaseException("Internal logic error in database engine") {}
	};

	class ErrPermission : public DatabaseException
	{
	public:
		ErrPermission() : DatabaseException("Access permission denied") {}
	};

	class ErrAbort : public DatabaseException
	{
	public:
		ErrAbort() : DatabaseException("Callback routine requested an abor") {}
	};

	class ErrBusy : public DatabaseException
	{
	public:
		ErrBusy() : DatabaseException("The database file is locked") {}
	};

	class ErrLocked : public DatabaseException
	{
	public:
		ErrLocked() : DatabaseException("A table in the database is locked") {}
	};

	class ErrNoMem : public DatabaseException
	{
	public:
		ErrNoMem() : DatabaseException("A memory alloctaion failed") {}
	};

	class ErrReadOnly : public DatabaseException
	{
	public:
		ErrReadOnly() : DatabaseException("Attempt to write a readonly database") {}
	};

	class ErrInterrupt : public DatabaseException
	{
	public:
		ErrInterrupt() : DatabaseException("Operation terminated by interrupt") {}
	};

	class ErrIO : public DatabaseException
	{
	public:
		ErrIO() : DatabaseException("Disk I/O error occurred") {}
	};

	class ErrCorrupt : public DatabaseException
	{
	public:
		ErrCorrupt() : DatabaseException("The database file has been corrupted") {}
	};

	class ErrNotFound : public DatabaseException
	{
	public:
		ErrNotFound() : DatabaseException("Table or record not found") {}
	};

	class ErrFull : public DatabaseException
	{
	public:
		ErrFull() : DatabaseException("Insertion failed because database is full") {}
	};

	class ErrCantOpen : public DatabaseException
	{
	public:
		ErrCantOpen() : DatabaseException("Unable to open the database file") {}
	};

	class ErrProtocol : public DatabaseException
	{
	public:
		ErrProtocol() : DatabaseException("Database lock protocol error") {}
	};

	class ErrEmpty : public DatabaseException
	{
	public:
		ErrEmpty() : DatabaseException("Database is empty") {}
	};

	class ErrSchema : public DatabaseException
	{
	public:
		ErrSchema() : DatabaseException("The database schema changed") {}
	};

	class ErrTooBig : public DatabaseException
	{
	public:
		ErrTooBig() : DatabaseException("Too much data for one row") {}
	};

	class ErrConstraint : public DatabaseException
	{
	public:
		ErrConstraint() : DatabaseException("Abort due to contraint violation") {}
	};

	class ErrMismatch : public DatabaseException
	{
	public:
		ErrMismatch() : DatabaseException("Data type mismatch") {}
	};

	class ErrMisuse : public DatabaseException
	{
	public:
		ErrMisuse() : DatabaseException("Library used incorrectly") {}
	};

	class ErrNoLFs : public DatabaseException
	{
	public:
		ErrNoLFs() : DatabaseException("OS features not supported on host") {}
	};

	class ErrAuth : public DatabaseException
	{
	public:
		ErrAuth() : DatabaseException("Authorization denied") {}
	};

	class ErrFormat : public DatabaseException
	{
	public:
		ErrFormat() : DatabaseException("Auxiliary database format error") {}
	};

	class ErrRange : public DatabaseException
	{
	public:
		ErrRange() : DatabaseException("Parameter out of range") {}
	};

	class ErrNotDB : public DatabaseException
	{
	public:
		ErrNotDB() : DatabaseException("File opened that is not a database file") {}
	};

}



#endif