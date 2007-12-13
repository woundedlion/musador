#ifndef ENTITY_FILE_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD
#define ENTITY_FILE_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD

#include <time.h>
#include <string>
#include "Database/Entity.h"

class EntityFile : public Entity
{
public:

	EntityFile(boost::shared_ptr<Database> db);
	~EntityFile();

	Column<std::wstring> filename;
	Column<time_t> mtime;
	Column<long> size;
	Column<long> parentID;
	Column<std::wstring> artist;
	Column<std::wstring> album;
	Column<std::wstring> title;
	Column<std::wstring> genre;
	Column<unsigned int> track;
	Column<int> length;
	Column<int> bitrate;
	Column<unsigned long> status_id;
};

#endif