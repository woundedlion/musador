#ifndef ENTITY_CONFIG_CATEGORY_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define ENTITY_CONFIG_CATEGORY_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include "Database/Entity.h"

namespace Musador
{
    namespace Config
    {
        /// @class EntityConfigCategory
        /// @brief A Database Entity representing a Configuration Category.
        class EntityConfigCategory : public Database::Entity
        {
        public:

            /// @brief The name of the category
            Database::Column<std::wstring> name;

            /// @brief The sort order for this category
            Database::Column<int> sortOrder;

            /// @brief Constructor.
            /// @param[in] db A shared pointer to the database instance where this Entity is stored.
            EntityConfigCategory::EntityConfigCategory(boost::shared_ptr<Database::IDatabase> db) : Entity(db, L"config_categories"),
                name(L"name"),
                sortOrder(L"sort_order")
            {
                bind(name);
                bind(sortOrder);
            }
        };
    }
}


#endif