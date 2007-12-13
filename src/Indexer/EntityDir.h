#ifndef ENTITY_DIR_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD
#define ENTITY_DIR_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD

#include <time.h>
#include <string>
#include <vector>
#include "Database/Entity.h"
#include "EntityFile.h"

class EntityDir : public Entity
{
public:

	Column<std::wstring> path;
	Column<time_t> mtime;

	EntityDir(boost::shared_ptr<Database> db);
	~EntityDir();

};

#endif