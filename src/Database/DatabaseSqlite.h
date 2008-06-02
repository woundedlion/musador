#ifndef DATABASESQLITE_5C44681D_25C3_4fce_BF1B_B491A877487D
#define DATABASESQLITE_5C44681D_25C3_4fce_BF1B_B491A877487D

#include <string>
#include <map>
#include <boost/thread/tss.hpp>
#include "Database.h"
#include "ResultSetSqlite.h"

struct sqlite3;

/// @class Implements the Database interface for sqlite3 databases.
class DatabaseSqlite : public Database
{
    friend class ResultSetSqlite;
public:

    /// Constructor.
    /// @param[in] databaseName The path to the database file on disk.
    DatabaseSqlite(std::wstring databaseName);

    /// Destructor.
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

    /// @class Exception type for sqlite-specific errors
    class ErrUnknown : public DatabaseException
    {
    public:

        /// Constructor.
        ErrUnknown() : DatabaseException("Unknown database error") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrSQL : public DatabaseException
    {
    public:
        
        /// Constructor.
        ErrSQL(const char * errMsg) : DatabaseException(errMsg ? errMsg : "SQL error or missing database" ) {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrInternal : public DatabaseException
    {
    public:

        /// Constructor.
        ErrInternal() : DatabaseException("Internal logic error in database engine") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrPermission : public DatabaseException
    {
    public:

        /// Constructor.
        ErrPermission() : DatabaseException("Access permission denied") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrAbort : public DatabaseException
    {
    public:

        /// Constructor.
        ErrAbort() : DatabaseException("Callback routine requested an abor") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrBusy : public DatabaseException
    {
    public:

        /// Constructor.
        ErrBusy() : DatabaseException("The database file is locked") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrLocked : public DatabaseException
    {
    public:

        /// Constructor.
        ErrLocked() : DatabaseException("A table in the database is locked") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrNoMem : public DatabaseException
    {
    public:

        /// Constructor.
        ErrNoMem() : DatabaseException("A memory alloctaion failed") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrReadOnly : public DatabaseException
    {
    public:

        /// Constructor.
        ErrReadOnly() : DatabaseException("Attempt to write a readonly database") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrInterrupt : public DatabaseException
    {
    public:

        /// Constructor.
        ErrInterrupt() : DatabaseException("Operation terminated by interrupt") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrIO : public DatabaseException
    {
    public:

        /// Constructor.
        ErrIO() : DatabaseException("Disk I/O error occurred") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrCorrupt : public DatabaseException
    {
    public:

        /// Constructor.
        ErrCorrupt() : DatabaseException("The database file has been corrupted") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrNotFound : public DatabaseException
    {
    public:

        /// Constructor.
        ErrNotFound() : DatabaseException("Table or record not found") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrFull : public DatabaseException
    {
    public:

        /// Constructor.
        ErrFull() : DatabaseException("Insertion failed because database is full") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrCantOpen : public DatabaseException
    {
    public:

        /// Constructor.
        ErrCantOpen() : DatabaseException("Unable to open the database file") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrProtocol : public DatabaseException
    {
    public:

        /// Constructor.
        ErrProtocol() : DatabaseException("Database lock protocol error") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrEmpty : public DatabaseException
    {
    public:

        /// Constructor.
        ErrEmpty() : DatabaseException("Database is empty") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrSchema : public DatabaseException
    {
    public:

        /// Constructor.
        ErrSchema() : DatabaseException("The database schema changed") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrTooBig : public DatabaseException
    {
    public:

        /// Constructor.
        ErrTooBig() : DatabaseException("Too much data for one row") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrConstraint : public DatabaseException
    {
    public:

        /// Constructor.
        ErrConstraint() : DatabaseException("Abort due to contraint violation") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrMismatch : public DatabaseException
    {
    public:

        /// Constructor.
        ErrMismatch() : DatabaseException("Data type mismatch") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrMisuse : public DatabaseException
    {
    public:

        /// Constructor.
        ErrMisuse() : DatabaseException("Library used incorrectly") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrNoLFs : public DatabaseException
    {
    public:

        /// Constructor.
        ErrNoLFs() : DatabaseException("OS features not supported on host") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrAuth : public DatabaseException
    {
    public:

        /// Constructor.
        ErrAuth() : DatabaseException("Authorization denied") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrFormat : public DatabaseException
    {
    public:

        /// Constructor.
        ErrFormat() : DatabaseException("Auxiliary database format error") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrRange : public DatabaseException
    {
    public:

        /// Constructor.
        ErrRange() : DatabaseException("Parameter out of range") {}
    };

    /// @class Exception type for sqlite-specific errors
    class ErrNotDB : public DatabaseException
    {
    public:

        /// Constructor.
        ErrNotDB() : DatabaseException("File opened that is not a database file") {}
    };

}



#endif