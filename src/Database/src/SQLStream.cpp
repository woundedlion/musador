#include "SQLStream.h"
#include "Column.h"

using namespace Musador::Database;

SQLStream& operator<<(SQLStream& s, const ColumnBase& col)
{
    col.toSQL(s);
    return s;
}