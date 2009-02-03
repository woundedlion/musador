#ifndef DATABASESQLITE_5C44681D_25C3_4fce_BF1B_B491A877487D
#define DATABASESQLITE_5C44681D_25C3_4fce_BF1B_B491A877487D

#include <string>
#include <map>
#include <boost/thread/tss.hpp>
#include "Database.h"
#include "ResultSetSqlite.h"

struct sqlite3;

namespace Musador
{
    namespace Database
    {
        /// @class DatabaseSqlite
        /// @brief Implements the IDatabase interface for sqlite3 databases.
        class DatabaseSqlite : public IDatabase
        {
            friend class ResultSetSqlite;

        public:

            /// @brief Constructor.
            /// @param[in] databaseName The path to the database file on disk.
            DatabaseSqlite(std::wstring databaseName);

            /// @brief Destructor.
            ~DatabaseSqlite();

            void txnBegin();
            void txnRollback();
            void txnCommit();

            boost::shared_ptr<ResultSet> select(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions);	
            id_t insert(const std::wstring& table, const std::vector<ColumnBase *>& columns);	
            void update(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions );	
            void remove(const std::wstring& table, const std::wstring& conditions);
            boost::shared_ptr<ResultSet> execute(const std::wstring& q);

        private:

            std::string dbName;
            boost::thread_specific_ptr<sqlite3> db;

            static void tssCleanup(sqlite3 * dbPtr);
            static void err(int errCode, const char * errMsg = NULL);

            bool open();
            bool close();
        };

        namespace sqlite {

            /// @class ErrUnknown
            /// @brief Exception type for sqlite-specific errors
            class ErrUnknown : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrUnknown() : DatabaseException("Unknown database error") {}
            };

            /// @class ErrSQL
            /// @brief Exception type for sqlite-specific errors
            class ErrSQL : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrSQL(const char * errMsg) : DatabaseException(errMsg ? errMsg : "SQL error or missing database" ) {}
            };

            /// @class ErrInternal
            /// @brief Exception type for sqlite-specific errors
            class ErrInternal : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrInternal() : DatabaseException("Internal logic error in database engine") {}
            };

            /// @class ErrPermission
            /// @brief Exception type for sqlite-specific errors
            class ErrPermission : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrPermission() : DatabaseException("Access permission denied") {}
            };

            /// @class ErrAbort
            /// @brief Exception type for sqlite-specific errors
            class ErrAbort : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrAbort() : DatabaseException("Callback routine requested an abor") {}
            };

            /// @class ErrBusy
            /// @brief Exception type for sqlite-specific errors
            class ErrBusy : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrBusy() : DatabaseException("The database file is locked") {}
            };

            /// @class ErrLocked
            /// @brief Exception type for sqlite-specific errors
            class ErrLocked : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrLocked() : DatabaseException("A table in the database is locked") {}
            };

            /// @class ErrNoMem
            /// @brief Exception type for sqlite-specific errors
            class ErrNoMem : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrNoMem() : DatabaseException("A memory alloctaion failed") {}
            };

            /// @class ErrReadOnly
            /// @brief Exception type for sqlite-specific errors
            class ErrReadOnly : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrReadOnly() : DatabaseException("Attempt to write a readonly database") {}
            };

            /// @class ErrInterrupt
            /// @brief Exception type for sqlite-specific errors
            class ErrInterrupt : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrInterrupt() : DatabaseException("Operation terminated by interrupt") {}
            };

            /// @class ErrIO
            /// @brief Exception type for sqlite-specific errors
            class ErrIO : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrIO() : DatabaseException("Disk I/O error occurred") {}
            };

            /// @class ErrCourrupt
            /// @brief Exception type for sqlite-specific errors
            class ErrCorrupt : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrCorrupt() : DatabaseException("The database file has been corrupted") {}
            };

            /// @class ErrNotFound
            /// @brief Exception type for sqlite-specific errors
            class ErrNotFound : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrNotFound() : DatabaseException("Table or record not found") {}
            };

            /// @class ErrFull
            /// @brief Exception type for sqlite-specific errors
            class ErrFull : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrFull() : DatabaseException("Insertion failed because database is full") {}
            };

            /// @class ErrCantOpen
            /// @brief Exception type for sqlite-specific errors
            class ErrCantOpen : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrCantOpen() : DatabaseException("Unable to open the database file") {}
            };

            /// @class ErrProtocol
            /// @brief Exception type for sqlite-specific errors
            class ErrProtocol : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrProtocol() : DatabaseException("Database lock protocol error") {}
            };

            /// @class ErrEmpty 
            /// @brief Exception type for sqlite-specific errors
            class ErrEmpty : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrEmpty() : DatabaseException("Database is empty") {}
            };

            /// @class ErrSchema 
            /// @brief Exception type for sqlite-specific errors
            class ErrSchema : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrSchema() : DatabaseException("The database schema changed") {}
            };

            /// @class ErrTooBig
            /// @brief Exception type for sqlite-specific errors
            class ErrTooBig : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrTooBig() : DatabaseException("Too much data for one row") {}
            };

            /// @class ErrConstraint
            /// @brief Exception type for sqlite-specific errors
            class ErrConstraint : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrConstraint() : DatabaseException("Abort due to contraint violation") {}
            };

            /// @class ErrMismatch
            /// @brief Exception type for sqlite-specific errors
            class ErrMismatch : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrMismatch() : DatabaseException("Data type mismatch") {}
            };

            /// @class ErrMisuse
            /// @brief Exception type for sqlite-specific errors
            class ErrMisuse : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrMisuse() : DatabaseException("Library used incorrectly") {}
            };

            /// @class ErrNoLFs
            /// @brief Exception type for sqlite-specific errors
            class ErrNoLFs : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrNoLFs() : DatabaseException("OS features not supported on host") {}
            };

            /// @class ErrAuth
            /// @brief Exception type for sqlite-specific errors
            class ErrAuth : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrAuth() : DatabaseException("Authorization denied") {}
            };

            /// @class ErrFormat
            /// @brief Exception type for sqlite-specific errors
            class ErrFormat : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrFormat() : DatabaseException("Auxiliary database format error") {}
            };

            /// @class ErrRange
            /// @brief Exception type for sqlite-specific errors
            class ErrRange : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrRange() : DatabaseException("Parameter out of range") {}
            };

            /// @class ErrNotDB
            /// @brief Exception type for sqlite-specific errors
            class ErrNotDB : public DatabaseException
            {
            public:

                /// @brief Constructor.
                ErrNotDB() : DatabaseException("File opened that is not a database file") {}
            };

        }
    }
}

#endif