#ifndef ENTITY_FILE_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD
#define ENTITY_FILE_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD

#include "Database/Entity.h"

namespace Musador
{
    /// @class EntityFile
    /// @brief A Database Entity representing a file.
    class EntityFile : public Database::Entity
    {
    public:

        /// @brief The filename of this file.
        Database::Column<std::wstring> filename;

        /// @brief The Last Modified time of this file.
        Database::Column<time_t> mtime;

        /// @brief The size, in bytes, of this file.
        Database::Column<long> size;

        /// @brief The foreign key ID for the directory containing this file.
        Database::Column<Database::id_t> parentID;

        /// @brief The Artist field present in this file's metadata
        Database::Column<std::wstring> artist;

        /// @brief The Album field present in this file's metadata
        Database::Column<std::wstring> album;

        /// @brief The Title field present in this file's metadata
        Database::Column<std::wstring> title;

        /// @brief The Genre field present in this file's metadata
        Database::Column<std::wstring> genre;

        /// @brief The Track field present in this file's metadata
        Database::Column<unsigned int> track;

        /// @brief The Length field present in this file's metadata
        Database::Column<int> length;

        /// @brief The Bitrate field present in this file's metadata
        Database::Column<int> bitrate;

        /// @brief The application-defined status of this file
        Database::Column<unsigned long> status_id;

        /// @brief Constructor.
        /// @param[in] db A shared pointer to the database instance where this Entity is stored.    
        EntityFile::EntityFile(boost::shared_ptr<Database::IDatabase> db) : Entity(db, L"files"),
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
    };
}
#endif