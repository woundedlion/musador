#ifndef RESULTSETSQLITE_5ED0E0D1_ED9A_4de0_931B_0448E601D090
#define RESULTSETSQLITE_5ED0E0D1_ED9A_4de0_931B_0448E601D090

#include "ResultSet.h"

struct sqlite3_stmt;

namespace Musador
{
    namespace Database
    {
        /// @class ResultSetSqlite
        /// @brief Implements the ResultSet interface for sqlite3 databases.
        class ResultSetSqlite : public ResultSet
        {
        public:

            /// @brief Constructor.
            /// @param[in] db Shared pointer to the IDatabase from which the ResultSet is drawn.
            /// @param[in] stmt A pointer to the sqlite3 statement object which is invoked to retrieve the ResultSet.
            ResultSetSqlite(boost::shared_ptr<IDatabase> db, sqlite3_stmt * stmt);

            /// @brief Destructor.
            ~ResultSetSqlite();

            unsigned long count();
            bool next();
            void reset();
            int getSize(int iCol) const;
            const unsigned char * getBlob(int iCol) const;
            double getDouble(int iCol) const;
            int getInt(int iCol) const;
            __int64 getInt64(int iCol) const;
            const char * getText(int iCol) const;

        private:

            sqlite3_stmt * stmt;

        };
    }
}

#endif