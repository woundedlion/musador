#include "Library.h"
#include "StatsBlock.h"
#include "Database/Database.h"

using namespace Musador;

Library::Library(LibraryConfig& cfg) :
cfg(cfg),
db(new Database::DatabaseSqlite(cfg.dataFile))
{

}

Library::~Library()
{

}

StatsBlock
Library::getCountsByGenre()
{
    StatsBlock b;
    b.id = this->cfg.id;
    b.displayName = this->cfg.nickname;
    std::wstring q(L"SELECT f.genre, count(f.id) FROM files AS f GROUP BY f.genre");
    boost::shared_ptr<Database::ResultSet> r = this->db->execute(q);
    if (NULL != r)
    {
        do
        {
            b.data[Util::utf8ToUnicode(r->getText(0))] = r->getInt(1);          
        } while (r->next());
    }
    return b;
}
