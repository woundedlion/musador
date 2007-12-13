#include "DatabaseSqlite.h"

ResultSetSqlite::ResultSetSqlite(boost::shared_ptr<Database> db, sqlite3_stmt * stmt) :
ResultSet(db),
stmt(stmt)
{
}

ResultSetSqlite::~ResultSetSqlite()
{
	// cleanup statement
	int err;
	if (SQLITE_OK != (err = ::sqlite3_finalize(this->stmt)))
	{
		DatabaseSqlite::err(err);
	}
}

unsigned long ResultSetSqlite::count()
{
	// TODO: Count all rows
	unsigned long r = 0;
	return r;
}

bool ResultSetSqlite::next()
{
	int err;

	// execute statement
	if (SQLITE_ROW == (err = ::sqlite3_step(stmt)))
	{
		return true;
	}
	if (SQLITE_DONE != err)
	{
		DatabaseSqlite::err(err);
	}
	return false;
}

void ResultSetSqlite::reset()
{
	::sqlite3_reset(this->stmt);
}

int ResultSetSqlite::getSize(int iCol) const 
{
	return ::sqlite3_column_bytes(this->stmt,iCol);
}

const unsigned char * ResultSetSqlite::getBlob(int iCol) const
{
	return static_cast<const unsigned char *>(::sqlite3_column_blob(this->stmt,iCol));
}

double ResultSetSqlite::getDouble(int iCol) const
{
	return ::sqlite3_column_double(this->stmt,iCol);
}

int ResultSetSqlite::getInt(int iCol) const
{
	return ::sqlite3_column_int(this->stmt,iCol);
}

__int64 ResultSetSqlite::getInt64(int iCol) const
{
	return ::sqlite3_column_int64(this->stmt,iCol);
}

const char * ResultSetSqlite::getText(int iCol) const
{
	return reinterpret_cast<const char *>(::sqlite3_column_text(this->stmt,iCol));
}
