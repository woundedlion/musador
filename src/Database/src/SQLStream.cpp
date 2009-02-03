#include "SQLStream.h"
#include "Column.h"

using namespace Musador::Database;

SQLStream& Musador::Database::operator<<(SQLStream& s, const ColumnBase& col)
{
    col.toSQL(s);
    return s;
}