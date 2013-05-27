#include "boost/lexical_cast.hpp"
#include "DatabaseSqlite.h"
#include "Utilities/Util.h"

namespace {
	void onError(int code) 
	{
		throw std::runtime_error(boost::lexical_cast<std::string>(code));
	}

	void onError(const char *msg) 
	{
		throw std::runtime_error(msg);
	}
}

using namespace storm::sqlite;

Database::Database(const std::wstring& dbName) : 
dbName(Util::unicodeToUtf8(dbName.c_str())),
db(NULL)
{
    open();
}

Database::~Database()
{
	close();
}

void
Database::open()
{
    if (db) return;
		
    int err = ::sqlite3_open(dbName.c_str(), &db);
	if (err != SQLITE_OK) {
		onError(err);
    }
}

void
Database::close()
{
    if (!db) return;

	int err = ::sqlite3_close(db);
	if (err != SQLITE_OK) {
        onError(err);
    }
	db = NULL;
}

//////////////////////////////////////////////////////////////////////////////

Statement::Statement(Database& db, const std::wstring& sql)
{
	prepare(db, sql);
}

void
Statement::prepare(Database& db, const std::wstring& sql)
{
	sqlite3_stmt *stmtPtr;
	const char *next;
	std::string sql8 = Util::unicodeToUtf8(sql);

	int err = ::sqlite3_prepare_v2(
		db, 
		sql8.c_str(), 
		static_cast<int>(sql8.size()),
		&stmtPtr, 
		&next);
    
	if (err != SQLITE_OK) {
		onError(err);
    }

	stmt.reset(stmtPtr, &sqlite3_finalize);
}

void
Statement::rewind()
{
	int err = ::sqlite3_reset(stmt.get());
	if (err != SQLITE_OK) {
		onError(err);
	}
}

//////////////////////////////////////////////////////////////////////////////

Row::Row() :
	stmt(NULL)
{}

Row::Row(const Statement& stmt) :
	stmt(&stmt)
{}

template <>
int
Row::get<int>(uint32_t col) const
{
	assert(stmt);
	return ::sqlite3_column_int(*stmt, static_cast<int>(col));
}

template <>
int64_t
Row::get<int64_t>(uint32_t col) const
{
	assert(stmt);
	return ::sqlite3_column_int64(*stmt, static_cast<int>(col));
}

template <>
double
Row::get<double>(uint32_t col) const
{
	assert(stmt);
	return ::sqlite3_column_double(*stmt, static_cast<int>(col));
}

template <>
std::string
Row::get<std::string>(uint32_t col) const
{
	assert(stmt);
	return std::string(reinterpret_cast<const char *>(::sqlite3_column_text(*stmt, static_cast<int>(col))));
}

template <>
std::wstring
Row::get<std::wstring>(uint32_t col) const
{
	assert(stmt);
	return Util::utf8ToUnicode(
		reinterpret_cast<const char *>(::sqlite3_column_text(*stmt, static_cast<int>(col))));
}

template <>
std::vector<unsigned char>
Row::get<std::vector<unsigned char>>(uint32_t col) const
{
	assert(stmt);
	auto b = static_cast<const unsigned char *>(::sqlite3_column_blob(*stmt, static_cast<int>(col)));
	auto e = b + ::sqlite3_column_bytes(*stmt, static_cast<int>(col)); 
	return std::vector<unsigned char>(b, e);
}

///////////////////////////////////////////////////////////////////////////////

ResultSet::ResultSet(const Statement& stmt) :
	stmt(stmt)
{}

ResultSet::~ResultSet()
{}

ResultSet::iterator
ResultSet::begin()
{
	stmt.rewind();
	iterator b(&stmt);
	b.increment();
	return b;
}

ResultSet::iterator
ResultSet::end()
{
	return iterator();
}

//////////////////////////////////////////////////////////////////////

ResultSet::iterator::iterator() :
	stmt(NULL)
{}

ResultSet::iterator::iterator(Statement *stmt) :
	stmt(stmt),
	row(*stmt)
{}

void
ResultSet::iterator::increment()
{
	assert(stmt);
	int err = ::sqlite3_step(*stmt);
	switch (err) {
	case SQLITE_ROW:
		break;
	case SQLITE_DONE:
		stmt = NULL;
		break;
	default:
		onError(err);
	}
}

bool
ResultSet::iterator::equal(const iterator& iter) const
{
	return iter.stmt == stmt;
}

const ResultSet::Row&
ResultSet::iterator::dereference() const
{
	assert(stmt);
	return row;
}

////////////////////////////////////////////////////////////////////////////////

Transaction::Transaction(Database& db) :
	db(db),
	committed(false)
{
	execute(L"BEGIN");	
}

Transaction::~Transaction()
{
	if (!committed) {
		execute(L"ROLLBACK");
	}
}

void
Transaction::commit()
{
	execute(L"COMMIT");
	committed = true;
}

void 
Transaction::execute(const std::wstring& sql)
{
	std::string sql8 = Util::unicodeToUtf8(sql);
	char *errMsgPtr = NULL;
	int err = sqlite3_exec(db, sql8.c_str(), NULL, NULL, &errMsgPtr);
	std::unique_ptr<char, std::function<void (void *)>> errMsg(errMsgPtr, &sqlite3_free);
	if (err != SQLITE_OK) {
		onError(errMsg.get());
	}
}

void 
Transaction::execute(Statement& stmt)
{
	int err = ::sqlite3_step(stmt);
	switch (err) {
	case SQLITE_DONE:
	case SQLITE_ROW:
		break;
	default:
		onError(err);
	}
	::sqlite3_reset(stmt);
}

ResultSet
Transaction::select(const std::wstring& sql)
{
	return select(Statement(db, sql));
}

ResultSet
Transaction::select(Statement& stmt)
{
	return ResultSet(stmt);
}

size_t
Transaction::update(const std::wstring& sql)
{
	execute(sql);
	return ::sqlite3_changes(db);
}

size_t
Transaction::update(Statement& stmt)
{
	execute(stmt);
	return ::sqlite3_changes(db);
}

id_t
Transaction::insert(const std::wstring& sql)
{
	execute(sql);
	return ::sqlite3_last_insert_rowid(db);
}

id_t
Transaction::insert(Statement& stmt)
{
	execute(stmt);
	return ::sqlite3_last_insert_rowid(db);
}
