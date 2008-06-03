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

namespace Musador
{
    namespace Database
    {
        /// @class IDatabase
        /// @brief Abstract base class for Database objects
        class IDatabase : public boost::enable_shared_from_this<IDatabase>
        {
        public:

            /// @brief Destructor.
            virtual ~IDatabase() {};

            /// @brief Begin a transaction on the database.
            virtual bool txnBegin() = 0;

            /// @brief Cancel a transaction on the database.
            virtual bool txnRollback() = 0;

            /// @brief Finalize a transaction on the database.
            virtual bool txnCommit() = 0;

            /// @brief Select matching rows from the database. 
            /// A SQL SELECT statement of the form SELECT <columns> FROM <table> WHERE <conditions> 
            /// is generated from the provided arguments.
            /// @param[in] table The database table from which to select.
            /// @param[in] columns A vector of columns to include in the SELECT statement.
            /// @param[in] conditions Condition string used for the WHERE clause in the SELECT statement.
            /// @returns A smart pointer to a ResultSet containing the matching rows.
            virtual std::auto_ptr<ResultSet> select(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions) = 0;	

            /// @brief Insert a row into the database.
            /// A SQL INSERT statement of the form INSERT INTO <table> (<columns_names>) VALUES (<column_values>) WHERE <consitions>
            /// is generated from the provided arguments.
            /// @param[in] table The database table in which to insert.
            /// @param[in] columns A vector of columns with data to include in the INSERT statement.
            /// @param[in] conditions Condition string used for the WHERE clause in the INSERT statement.
            /// @returns The id value generated for the new row
            virtual unsigned long insert(const std::wstring& table, const std::vector<ColumnBase *>& columns) = 0;	

            /// @brief Update rows in the database.
            /// A SQL UPDATE statement of the form UPDATE <table> SET (<column_name>=<column_value>, ...) WHERE <conditions>
            /// is generated from the provided arguments.
            /// @param[in] table The database table on which to update.
            /// @param[in] columns A vector of columns with data to include in the UPDATE statement.
            /// @param[in] conditions Condition string used for the WHERE clause in the UPDATE statement.
            /// @returns true if the query succeeded, false otherwise
            virtual bool update(const std::wstring& table, const std::vector<ColumnBase *>& columns, const std::wstring& conditions ) = 0;	

            /// @brief Delete rows from the database.
            /// A SQL DELETE statement of the form DELETE FROM <table> WHERE <conditions>
            /// is generated from the provided arguments.
            /// @param[in] table The database table from which to delete.
            /// @param[in] conditions Condition string used for the WHERE clause in the DELETE statement.
            /// @returns true if the query succeeded, false otherwise
            virtual bool remove(const std::wstring& table, const std::wstring& conditions) = 0;

            /// @brief Execute an arbitrary SQL query on the database.
            /// @param[in] q A string containing the query to execute.
            /// @returns true if the query succeeded, false otherwise
            virtual bool execute(const std::wstring& q) = 0;

        protected:

            IDatabase() {};

        };

        /// @class DatabaseException
        /// @brief Base class for database-related exceptions
        class DatabaseException : public std::runtime_error
        {
        public:

            /// @brief Constructor.
            DatabaseException(const char * err) : std::runtime_error(err) {	}

            /// @brief Destructor.
            virtual ~DatabaseException() {};

        };
    }
}

#include "Entity.h"
#include "DatabaseSqlite.h"

#endif