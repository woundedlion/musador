#ifndef RESULTSET_5ED0E0D1_ED9A_4de0_931B_0448E601D090
#define RESULTSET_5ED0E0D1_ED9A_4de0_931B_0448E601D090

#include <vector>
#include "boost/shared_ptr.hpp"
//#include "Database.h"

namespace Musador
{
    namespace Database
    {
        class IDatabase;

        /// @class ResultSet
        /// @brief A collection of results from a call to IDatabase::select()
        class ResultSet
        {
        public:

            /// @brief Constructor.
            /// @param[in] db Shared pointer to the IDatabase from which the ResultSet is drawn.
            ResultSet(boost::shared_ptr<IDatabase> db);

            /// @brief Destructor.
            virtual ~ResultSet() {};

            /// @brief Get a shared pointer to the IDatabase from which these results come.
            /// @returns a shared pointer to the IDatabase from which these results come.
            boost::shared_ptr<IDatabase> getDb();

            /// @brief Get the count of items in the result set.
            /// @returns the count of items in the result set.
            virtual unsigned long count() = 0;

            /// @brief Increment the current row index to the next row in the ResultSet.
            /// @returns true on success, false if there are no more rows in the ResultSet.
            virtual bool next() = 0;

            /// @brief Reset the current row index to the first row in the ResultSet
            virtual void reset() = 0;

            /// @brief Get the size, in bytes, of the data in the specified column of the current row in the ResultSet.
            /// @param[in] iCol Zero-based integer index of the column in question.
            /// @returns The size in bytes of the data in the specified column.
            virtual int getSize(int iCol) const = 0;

            /// @brief Get binary data from the specified column in the current row of the ResultSet.
            /// @param[in] iCol Zero-based integer index of the column in question.
            /// @returns a pointer to an array of bytes. 
            /// @remarks The pointer returned should not be freed by the caller. It is valid until either the
            /// current row changes via a call to next() or reset() or the ResultSet id deleted.
            virtual const unsigned char * getBlob(int iCol) const = 0;

            /// @brief Get a double value from the specified column in the current row of the ResultSet.
            /// @param[in] iCol Zero-based integer index of the column in question.
            /// @returns the value of the specified column. 
            virtual double getDouble(int iCol) const = 0;

            /// @brief Get an integer value from the specified column in the current row of the ResultSet.
            /// @param[in] iCol Zero-based integer index of the column in question.
            /// @returns the value of the specified column. 
            virtual int getInt(int iCol) const = 0;

            /// @brief Get a 64-bit integer value from the specified column in the current row of the ResultSet.
            /// @param[in] iCol Zero-based integer index of the column in question.
            /// @returns the value of the specified column. 
            virtual __int64 getInt64(int iCol) const = 0;

            /// @brief Get character data from the specified column in the current row of the ResultSet.
            /// @param[in] iCol Zero-based integer index of the column in question.
            /// @returns a pointer to an array of characters. 
            /// @remarks The pointer returned should not be freed by the caller. It is valid until either the
            /// current row changes via a call to next() or reset() or the ResultSet id deleted.
            virtual const char * getText(int iCol) const = 0;

        private:

            boost::shared_ptr<IDatabase> db;
        };
    }
}

#endif