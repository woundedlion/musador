#ifndef ENTITY_2CC11132_9780_4998_A779_3327320D5037
#define ENTITY_2CC11132_9780_4998_A779_3327320D5037

#include <vector>
#include <boost/smart_ptr.hpp>
#include "Database.h"

/// @class representing a row in a database.
class Entity
{
public:

    friend class DatabaseTest;

    /// Constructor.
    /// param[in] dbPtr Shared pointer to the Database in which this Entity is stored.
    /// param[in] table The name of the table in which this Entity is stored.
    Entity(boost::shared_ptr<Database> dbPtr, std::wstring table);

    /// Destructor.
    virtual ~Entity() = 0;

    /// Get the numeric id of this row.
    /// @returns the numeric ID of this row.
    inline unsigned long getId() { return this->id; }

    /// Populate this Entity instance with data from the specified row in the database.
    /// @param[in] id The numeric ID of the row in the database used to populate this Entity.
    /// @returns true on success, false otherwise
    virtual bool load(unsigned long id);

    /// Insert or Update the corresponding row in the database with the values contained in this Entity.
    /// Note that only fields which have been changed (marked dirty) will be included
    /// in the save.
    /// @returns true on success, false otherwise
    virtual bool save();

    /// Delete the corresponding row from the database, as specified by table name and ID.
    /// @returns true on success, false otherwise
    virtual bool del();

protected:

    inline void reg(ColumnBase * col)
    {
        this->columns.push_back(col);
    }

    boost::shared_ptr<Database> db;
    std::wstring table;
    unsigned long id;
    std::vector<ColumnBase *> columns;

};

#endif