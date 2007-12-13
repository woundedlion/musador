#include "EntityDir.h"

EntityDir::EntityDir(boost::shared_ptr<Database> db) :
	Entity(db, L"dirs"),
	// bind members to column names in the database table
	path(L"path"),
	mtime(L"mtime")
{
	// register members 
	reg(&path);
	reg(&mtime);
}

EntityDir::~EntityDir()
{

}

