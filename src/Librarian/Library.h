#ifndef LIBRARY_H_F34BA23D_EBB0_4871_89C1_20AB9FDC155D
#define LIBRARY_H_F34BA23D_EBB0_4871_89C1_20AB9FDC155D

#include <string>
#include <vector>
#include "Config/Config.h"
#include "Database/DatabaseSqlite.h"

namespace Musador
{
    class LibraryConfig;
    class StatsBlock;

    class Library
    {
    public:
        
        Library(LibraryConfig& cfg);
        ~Library();
        StatsBlock getCountsByGenre();

    private:

        LibraryConfig& cfg;
		storm::sqlite::Database db;
    };
}

#endif
