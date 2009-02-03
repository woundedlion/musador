#ifndef COLUMN_13B33D06_6101_44ba_A429_9A9FAC650E0B
#define COLUMN_13B33D06_6101_44ba_A429_9A9FAC650E0B

#include <sstream>
#include "Utilities/Util.h"
#include "ResultSet.h"
#include "SQLStream.h"
#include "Database.h"

namespace Musador
{
    namespace Database
    {
        /// @class ColumnBase
        /// @brief Base class for columns in a Database Entity
        class ColumnBase
        {
        public:

            /// @brief Constructor.
            /// @param[in] name The name of the column in the underlying database.
            explicit ColumnBase(std::wstring name) : 
            nameStr(name),
                dirty(false)
            {}

            /// @brief Destructor.
            virtual ~ColumnBase() 
            {}

            /// @brief Get the name of the Column.
            /// @returns a wide-character string containing the name of the Column.
            inline const std::wstring& name()
            {
                return this->nameStr;
            }

            /// @brief Check whether a column is dirty, i.e. has changed since the last save to the database.
            /// @returns True if the column has changed and requires a write to the databse, false otherwise
            inline bool isDirty()
            {
                return this->dirty;
            }

            /// @brief Set the dirty flag on a column so that it will be written to the database on the next save.
            /// @param d Boolean value used to set the dirty flag for this column.
            inline void setDirty(bool d)
            {
                this->dirty = d;
            }

            /// @brief Write the value contained in this column to the specified SQLStream.
            /// This function qutomatically performs any escaping, quoting, or encoding conversion 
            /// appropriate to the contained type.
            /// @param[in] out The stream to which the value is written
            virtual void toSQL(SQLStream& out) const = 0;

            /// @brief Fills this column with the value from a particular column in a ResultSet.
            /// @param[in] r A ResultSet containing column data retrieved from the database.
            /// @param[in] iCol Zero-based integer column index into the ResultSet specifying the column from 
            /// which to extract the data.
            virtual void assign(const ResultSet& r, int iCol) = 0;

        private:

            std::wstring nameStr;
            bool dirty;

        };

        /// @class Column
        /// @brief Concrete class template for various Column types.
        /// Implements the interface defined in ColumnBase
        template <typename T>
        class Column : public ColumnBase
        {
        public:

            Column(const std::wstring& name);

            ~Column();

            Column<T>& operator=(const T& rhs);	

            bool operator==(const T& rhs);

            inline operator T() { return data; }

            void toSQL(SQLStream& out) const;

            void assign(const ResultSet& r, int iCol);

        private:

            T data;

        };

        template <typename T>
        Column<T>::Column(const std::wstring& name):
        ColumnBase(name),
            data(T())
        {
        }

        template <typename T>
        Column<T>::~Column()
        {

        }

        template <typename T>
        Column<T>& Column<T>::operator=(const T& rhs)
        {
            this->data = rhs;
            this->setDirty(true);
            return *this;
        }

        template <typename T>
        inline bool Column<T>::operator==(const T& rhs)
        {
            return this->data == rhs;
        }

        // Template specializations


        template <typename T>
        inline void Column<T>::toSQL(SQLStream& out) const
        {
            out << this->data;
        }

        template <>
        inline void Column<std::string>::toSQL(SQLStream& out) const
        {
            out << L'\'' << Util::escapeQuotes(Util::utf8ToUnicode(this->data.c_str())) << '\'';
        }

        template <>
        inline void Column<char *>::toSQL(SQLStream& out) const
        {
            out << L'\'' << Util::escapeQuotes(Util::utf8ToUnicode(this->data)) << L'\'';
        }

        template <>
        inline void Column<std::wstring>::toSQL(SQLStream& out) const
        {
            out << L'\'' << Util::escapeQuotes(this->data) << L'\'';
        }

        template <>
        inline void Column<wchar_t *>::toSQL(SQLStream& out) const
        {
            out << L'\'' << Util::escapeQuotes(this->data) << L'\'';
        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        template <>
        inline void Column<std::string>::assign(const ResultSet& r, int iCol)
        {
            const char * bytes = r.getText(iCol);
            if (NULL != bytes)
            {
                *this = bytes;
            }
        }

        template <>
        inline void Column<std::wstring>::assign(const ResultSet& r, int iCol)
        {
            const char * bytes = r.getText(iCol);
            if (NULL != bytes)
            {
                *this = Util::utf8ToUnicode(bytes);
            }
        }

        template <>
        inline void Column<char *>::assign(const ResultSet& r, int iCol)
        {
            const char * bytes = r.getText(iCol);
            if (NULL != bytes)
            {
                ::strcpy(this->data,bytes);
            }
        }

        template <>
        inline void Column<wchar_t *>::assign(const ResultSet& r, int iCol)
        {
            const char * bytes = r.getText(iCol);
            if (NULL != bytes)
            {
                std::wstring wStr = Util::utf8ToUnicode(bytes);
                ::wcscpy(this->data, wStr.c_str());
            }
        }

        template <>
        inline void Column<unsigned char *>::assign(const ResultSet& r, int iCol)
        {
            const unsigned char * bytes = r.getBlob(iCol);
            if (NULL != bytes)
            {
                ::memcpy(this->data,bytes,r.getSize(iCol));
            }
        }

        template <>
        inline void Column<unsigned int>::assign(const ResultSet& r, int iCol)
        {
            *this = static_cast<unsigned int>(r.getInt(iCol));
        }

        template <>
        inline void Column<int>::assign(const ResultSet& r, int iCol)
        {
            *this = r.getInt(iCol);
        }

        template <>
        inline void Column<long>::assign(const ResultSet& r, int iCol)
        {
            *this = r.getInt(iCol);
        }

        template <>
        inline void Column<unsigned long>::assign(const ResultSet& r, int iCol)
        {
            *this = r.getInt(iCol);
        }

        template <>
        inline void Column<double>::assign(const ResultSet& r, int iCol)
        {
            *this = r.getDouble(iCol);
        }

        template <>
        inline void Column<id_t>::assign(const ResultSet& r, int iCol)
        {
            *this = r.getInt64(iCol);
        }
    }
}

#endif