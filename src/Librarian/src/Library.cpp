#include "Library.h"
#include "StatsBlock.h"
#include "Utilities/Util.h"
#include "Librarian/Config.h"

using namespace Musador;
using namespace storm::sqlite;

Library::Library(LibraryConfig& cfg) :
cfg(cfg),
db(cfg.dataFile)
{

}

Library::~Library()
{

}

StatsBlock
Library::getCountsByGenre()
{
    StatsBlock b;
    b.id = cfg.id;
    b.displayName = cfg.nickname;
    std::wstring q(L"SELECT f.genre, count(f.id) FROM files AS f GROUP BY f.genre");
    Transaction txn(db);
	auto counts = txn.select(q);
	for (auto count : counts) {
		b.data[Util::utf8ToUnicode(count.get<std::string>(0))] = count.get<int>(1);          
    }
    return b;
}
