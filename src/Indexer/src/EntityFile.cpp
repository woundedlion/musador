#include "EntityFile.h"

using namespace Musador;

EntityFile::EntityFile(boost::shared_ptr<Database::IDatabase> db) :
	Entity(db, L"files"),
	// bind members to column names in the database table
	filename(L"filename"),
	size(L"size"),
	mtime(L"mtime"),
	parentID(L"parent_id"),
	artist(L"artist"),
	album(L"album"),
	title(L"title"),
	genre(L"genre"),
	track(L"track"),
	length(L"length"),
	bitrate(L"bitrate"),
	status_id(L"status_id")
{
	// register members 
	reg(&filename);
	reg(&size);
	reg(&mtime);
	reg(&parentID);
	reg(&artist);
	reg(&album);
	reg(&title);
	reg(&genre);
	reg(&track);
	reg(&length);
	reg(&bitrate);
	reg(&status_id);
}

EntityFile::~EntityFile()
{

}

