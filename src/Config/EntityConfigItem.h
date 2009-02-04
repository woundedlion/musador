#ifndef ENTITY_CONFIG_ITEM_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45
#define ENTITY_CONFIG_ITEM_H_24DF554B_8EF1_40c0_8056_F349BC8E3F45

#include "Database/Entity.h"

namespace Musador
{
    namespace Config
    {
        /// @class EntityConfigItem
        /// @brief A Database Entity representing a Configuration Item.
        class EntityConfigItem : public Database::Entity
        {
        public:

            /// @brief The name of this config item
            Database::Column<std::wstring> name;

            /// @brief The value of this config item
            Database::Column<unsigned char *> value;

            /// @brief The sortOrder for this config item
            Database::Column<int> sortOrder;

            /// @brief Constructor.
            /// @param[in] db A shared pointer to the database instance where this Entity is stored.
            EntityConfigItem::EntityConfigItem(boost::shared_ptr<Database::IDatabase> db) : Entity(db, L"config_items"),
                name(L"name"),
                value(L"value"),
                sortOrder(L"sort_order")
            {
                bind(name);
                bind(value);
                bind(sortOrder);
            }
        };
    }
}

#endif