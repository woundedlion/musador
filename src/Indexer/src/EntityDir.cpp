#include "EntityDir.h"

using namespace Musador;

EntityDir::EntityDir(boost::shared_ptr<Database::IDatabase> db) :
Entity(db, L"dirs"),
path(L"path"),
mtime(L"mtime")
{
    bind(path);
    bind(mtime);
}

EntityDir::~EntityDir()
{

}

