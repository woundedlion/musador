#ifndef DATABASE_5C44681D_25C3_4fce_BF1B_B491A877487D
#define DATABASE_5C44681D_25C3_4fce_BF1B_B491A877487D

#include <string>
#include <vector>
#include <stdexcept>
#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Column.h"
#include "ResultSet.h"

#define NEW_ROW_ID -1

class Database : public boost::enable_shared_from_this<Database>
{
public:

	virtual ~Database() {};

	virtual bool txnBegin() = 0;
	virtual bool txnRollback() = 0;
	virtual bool txnCommit() = 0;

	virtual std::auto_ptr<ResultSet> select(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions) = 0;	
	virtual unsigned long insert(const std::wstring& table, const std::vector<ColumnBase *>& columns) = 0;	
	virtual bool update(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions ) = 0;	
	virtual bool remove(const std::wstring& table, const std::wstring& conditions) = 0;
	virtual bool execute(const std::wstring& q) = 0;

protected:

	Database() {};

};

class DatabaseException : public std::runtime_error
{
public:

	DatabaseException(const char * err) : std::runtime_error(err) {	}
	virtual ~DatabaseException() {};

};

#include "Entity.h"
#include "DatabaseSqlite.h"

#endif