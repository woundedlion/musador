#ifndef ENTITY_2CC11132_9780_4998_A779_3327320D5037
#define ENTITY_2CC11132_9780_4998_A779_3327320D5037

#include <vector>
#include <boost/smart_ptr.hpp>
#include "Database.h"

namespace Musador
{
    namespace Database
    {
        /// @class Entity
        /// @brief Represents a row in a database.
        class Entity
        {
        public:

            friend class DatabaseTest;

            /// @brief Constructor.
            /// @param[in] dbPtr Shared pointer to the IDatabase in which this Entity is stored.
            /// @param[in] table The name of the table in which this Entity is stored.
            Entity(boost::shared_ptr<IDatabase> dbPtr, std::wstring table);

            /// @brief Destructor.
            virtual ~Entity() = 0;

            /// @brief Get the numeric id of this row.
            /// @returns the numeric ID of this row.
            inline __int64 getId() { return this->id; }

            /// @brief Populate this Entity instance with data from the specified row in the database.
            /// @param[in] id The numeric ID of the row in the database used to populate this Entity.
            virtual void load(__int64 id);

            /// @brief Insert or Update the corresponding row in the database with the values contained in this Entity.
            /// Note that only fields which have been changed (marked dirty) will be included
            /// in the save.
            virtual void save();

            /// @brief Delete the corresponding row from the database, as specified by table name and ID.
            virtual void del();

        protected:

            inline void reg(ColumnBase * col)
            {
                this->columns.push_back(col);
            }

            boost::shared_ptr<IDatabase> db;
            std::wstring table;
            __int64 id;
            std::vector<ColumnBase *> columns;

        };
    }
}

#endif