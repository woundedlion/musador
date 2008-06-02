#ifndef ENTITY_FILE_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD
#define ENTITY_FILE_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD

#include <time.h>
#include <string>
#include "Database/Entity.h"

/// @class EntityFile
/// @brief A Database Entity representing a file.
class EntityFile : public Entity
{
public:

    /// @brief Constructor.
    /// @param[in] db A shared pointer to the database instance where this Entity is stored.    
    EntityFile(boost::shared_ptr<Database> db);

    /// @brief Destructor.
    ~EntityFile();

    /// @brief The filename of this file.
    Column<std::wstring> filename;

    /// @brief The Last Modified time of this file.
    Column<time_t> mtime;

    /// @brief The size, in bytes, of this file.
    Column<long> size;

    /// @brief The foreign key ID for the directory containing this file.
    Column<long> parentID;

    /// @brief The Artist field present in this file's metadata
    Column<std::wstring> artist;

    /// @brief The Album field present in this file's metadata
    Column<std::wstring> album;

    /// @brief The Title field present in this file's metadata
    Column<std::wstring> title;

    /// @brief The Genre field present in this file's metadata
    Column<std::wstring> genre;

    /// @brief The Track field present in this file's metadata
    Column<unsigned int> track;

    /// @brief The Length field present in this file's metadata
    Column<int> length;

    /// @brief The Bitrate field present in this file's metadata
    Column<int> bitrate;

    /// @brief The application-defined status of this file
    Column<unsigned long> status_id;
};

#endif