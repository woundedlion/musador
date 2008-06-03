#ifndef ENTITY_DIR_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD
#define ENTITY_DIR_AA8ED717_7E19_4831_A696_C5C8ECB2B3FD

#include <time.h>
#include <string>
#include <vector>
#include "Database/Entity.h"
#include "EntityFile.h"

namespace Musador
{
    /// @class EntityDir
    /// @brief A Database Entity representing a directory.
    class EntityDir : public Database::Entity
    {
    public:

        /// @brief Constructor.
        /// @param[in] db A shared pointer to the database instance where this Entity is stored.
        EntityDir(boost::shared_ptr<Database::IDatabase> db);

        /// @brief Destructor.
        ~EntityDir();

        /// @brief The full path to the directory on disk
        Database::Column<std::wstring> path;

        /// @brief The Last Modified time of the directory
        Database::Column<time_t> mtime;

    };
}
#endif