#include <sstream>
#include <algorithm>
#include "Utilities/Util.h"
#include "DatabaseSqlite.h"
#include "sqlite/sqlite3.h"

using namespace Musador::Database;

DatabaseSqlite::DatabaseSqlite(std::wstring databaseName) : 
	dbName(Util::unicodeToUtf8(databaseName.c_str())),
		db(&DatabaseSqlite::tssCleanup)
{
	this->open();
}

DatabaseSqlite::~DatabaseSqlite()
{
	try
	{
		this->close();
	}
	catch (DatabaseException)
	{
		std::abort();
	}
}

bool 
DatabaseSqlite::txnBegin()
{
	return this->execute(L"BEGIN");	
}

bool 
DatabaseSqlite::txnRollback()
{
	return this->execute(L"ROLLBACK");	
}

bool 
DatabaseSqlite::txnCommit()
{
	return this->execute(L"COMMIT");	
}

bool 
DatabaseSqlite::open()
{
	if (NULL != this->db.get())
		return false;

	int err;
	sqlite3 * dbPtr;
	if (SQLITE_OK != (err = ::sqlite3_open(this->dbName.c_str(),&dbPtr)))
	{
		DatabaseSqlite::err(err);
	}
	this->db.reset(dbPtr);
	return true;
}

bool 
DatabaseSqlite::close()
{
	if (NULL == this->db.get())
		return false;
	
	int err;
	if (SQLITE_OK != (err = ::sqlite3_close(this->db.get())))
	{
		DatabaseSqlite::err(err);
	}
	
	this->db.release();
	return true;
}

void 
DatabaseSqlite::tssCleanup(sqlite3 * dbPtr)
{
	int err;
	if (dbPtr && SQLITE_OK != (err = ::sqlite3_close(dbPtr)))
	{
		DatabaseSqlite::err(err);
	}

	delete dbPtr;
}

std::auto_ptr<ResultSet> 
DatabaseSqlite::select(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions)	
{
	std::wstringstream q;
	q << "SELECT ";
	if (columns.empty())
		q << "*";
	else
	{
		bool first = true;
		for (std::vector<ColumnBase *>::const_iterator iter = columns.begin(); iter != columns.end(); iter++)
		{
			if (first)
				first = false;
			else
				q << ",";
			q << (*iter)->name().c_str();
		}
	}

	q << " FROM " << table.c_str();
	
	if (!conditions.empty())
		q << " WHERE " << conditions.c_str();


	sqlite3_stmt * stmt;
	const char * next;
	int err;
	std::string qUtf8 = Util::unicodeToUtf8(q.str().c_str());
	// prepare statement
	if (SQLITE_OK != (err = ::sqlite3_prepare_v2(this->db.get(),qUtf8.c_str(), static_cast<int>(qUtf8.size()),&stmt, &next)))
	{
		DatabaseSqlite::err(err);
	}

	std::auto_ptr<ResultSet> result(new ResultSetSqlite(this->shared_from_this(),stmt)); 
	
	return result;
}

unsigned long 
DatabaseSqlite::insert(const std::wstring& table,  const std::vector<ColumnBase *>& columns)
{
	std::wstringstream q;
	std::wstringstream valueStr;
	q << "INSERT INTO " << table.c_str() << "(";
	bool first = true;
	for (std::vector<ColumnBase *>::const_iterator iter = columns.begin(); iter != columns.end(); iter++)
	{
		if (first)
			first = false;
		else
		{
			q << ",";
			valueStr << ",";
		}
		q << (*iter)->name();
		(*iter)->writeValue(valueStr);
	}
	q << ") VALUES (" << valueStr.str() << ")";

	return (this->execute(q.str()) ? static_cast<unsigned long>(::sqlite3_last_insert_rowid(this->db.get())) : NEW_ROW_ID );
}

bool 
DatabaseSqlite::update(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions )
{
	std::wstringstream q;
	q << "UPDATE " << table << " SET ";
	bool first = true;
	for (std::vector<ColumnBase *>::const_iterator iter = columns.begin(); iter != columns.end(); iter++)
	{
		if (first)
			first = false;
		else
		{
			q << ",";
		}
		q << (*iter)->name().c_str() << L"=";
		(*iter)->writeValue(q);
	}
		
	if (!conditions.empty())
		q << " WHERE " << conditions;

	return this->execute(q.str());
}

bool 
DatabaseSqlite::remove(const std::wstring& table, const std::wstring& conditions)
{
	std::wstringstream q;
	q << "DELETE FROM " << table << " WHERE " << conditions;
	return this->execute(q.str());
}

bool 
DatabaseSqlite::execute(const std::wstring& q)
{
	if (NULL == this->db.get())
		return false;

	char * errMsg = NULL;
	int err;
	if (SQLITE_OK != (err = ::sqlite3_exec(this->db.get(),Util::unicodeToUtf8(q.c_str()).c_str(),NULL,NULL,&errMsg)))
	{
		DatabaseSqlite::err(err,(std::string(errMsg) + ": " + Util::unicodeToUtf8(q.c_str())).c_str());
	}
	
	return true;
}

void 
DatabaseSqlite::err(int errCode, const char * errMsg)
{
	switch (errCode)
	{
	case 1:
		throw sqlite::ErrSQL(errMsg);
		break;
	case 2:
		throw sqlite::ErrInternal();
		break;
	case 3:
		throw sqlite::ErrPermission();
		break;
	case 4:
		throw sqlite::ErrAbort();
		break;
	case 5:
		throw sqlite::ErrBusy();
		break;
	case 6:
		throw sqlite::ErrLocked();
		break;
	case 7:
		throw sqlite::ErrNoMem();
		break;
	case 8:
		throw sqlite::ErrReadOnly();
		break;
	case 9:
		throw sqlite::ErrInterrupt();
		break;
	case 10:
		throw sqlite::ErrIO();
		break;
	case 11:
		throw sqlite::ErrCorrupt();
		break;
	case 12:
		throw sqlite::ErrNotFound();
		break;
	case 13:
		throw sqlite::ErrFull();
		break;
	case 14:
		throw sqlite::ErrCantOpen();
		break;
	case 15:
		throw sqlite::ErrProtocol();
		break;
	case 16:
		throw sqlite::ErrEmpty();
		break;
	case 17:
		throw sqlite::ErrSchema();
		break;
	case 18:
		throw sqlite::ErrTooBig();
		break;
	case 19:
		throw sqlite::ErrConstraint();
		break;
	case 20:
		throw sqlite::ErrMismatch();
		break;
	case 21:
		throw sqlite::ErrMisuse();
		break;
	case 22:
		throw sqlite::ErrNoLFs();
		break;
	case 23:
		throw sqlite::ErrAuth();
		break;
	case 24:
		throw sqlite::ErrFormat();
		break;
	case 25:
		throw sqlite::ErrRange();
		break;
	case 26:
		throw sqlite::ErrNotDB();
		break;
	default:
		throw sqlite::ErrUnknown();
	}
}


























