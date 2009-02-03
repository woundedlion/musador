#include <sstream>
#include <boost/shared_ptr.hpp>
#include "Utilities/Util.h"
#include "Entity.h"

using namespace Musador::Database;

Entity::Entity(boost::shared_ptr<IDatabase> dbPtr, std::wstring table) :
db(dbPtr),
table(table),
id(NEW_ROW_ID)
{

}

Entity::~Entity()
{

}

void 
Entity::load(id_t id)
{
    this->id = id;
    std::wstringstream cond;
    cond << L"id=" << this->id;
    boost::shared_ptr<ResultSet> r = this->db->select(table,this->columns,cond.str());
    if (NULL == r)
    {
        return;
    }

    int i = 0;
    for (std::vector<ColumnBase *>::iterator iter = this->columns.begin(); iter != this->columns.end(); iter++)
    {
        (*iter)->assign(*r.get(),i);
        (*iter)->setDirty(false);
        ++i;
    }
}

void 
Entity::save()
{
    // INSERT if id is NULL
    if (NEW_ROW_ID == this->id)
    {
        this->id = this->db->insert(table,this->columns);
    }
    else
    {
        // otherwise UPDATE only dirty columns
        std::vector<ColumnBase *> cols;
        for (std::vector<ColumnBase *>::iterator iter = this->columns.begin(); iter != this->columns.end(); iter++)
        {
            if ((*iter)->isDirty())
            {
                cols.push_back(*iter);
            }
        }

        std::wstringstream cond;
        cond << L"id=" << this->id;
        this->db->update(table,cols,cond.str());
        for (std::vector<ColumnBase *>::iterator iter = cols.begin(); iter != cols.end(); iter++)
        {
            (*iter)->setDirty(false);
        }
    }
}

void 
Entity::del()
{
    std::wstringstream cond;
    cond << L"id=" << this->id;
    this->db->remove(table,cond.str());
}