#ifndef SQL_STREAM_H_97BE0F76_96CD_405c_97C7_2FF1E0B17C11
#define SQL_STREAM_H_97BE0F76_96CD_405c_97C7_2FF1E0B17C11

#include <sstream>

namespace Musador
{
    namespace Database
    {
        typedef std::wstringstream SQLStream;
        class ColumnBase;

        SQLStream& operator<<(SQLStream& s, const ColumnBase& col);
    }
}

#endif