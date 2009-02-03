#include "EntityFile.h"

using namespace Musador;

EntityFile::EntityFile(boost::shared_ptr<Database::IDatabase> db) :
Entity(db, L"files"),
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
    bind(filename);
    bind(size);
    bind(mtime);
    bind(parentID);
    bind(artist);
    bind(album);
    bind(title);
    bind(genre);
    bind(track);
    bind(length);
    bind(bitrate);
    bind(status_id);
}

EntityFile::~EntityFile()
{

}

