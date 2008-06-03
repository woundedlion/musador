#include <sstream>
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

bool 
Entity::load(unsigned long id)
{
	this->id = id;
	std::wstringstream cond;
	cond << L"id=" << this->id;
	std::auto_ptr<ResultSet> r = this->db->select(table,this->columns,cond.str());
	// Execute the query once, r now holds the first matching row 
	if (!r->next())
		return false;

	int i = 0;
	for (std::vector<ColumnBase *>::iterator iter = this->columns.begin(); iter != this->columns.end(); iter++)
	{
		(*iter)->extractResult(r.get(),i);
		(*iter)->setDirty(false);
		++i;
	}

	return true;
}

bool 
Entity::save()
{
	// INSERT if id is NULL
	if (NEW_ROW_ID == this->id)
		return (NEW_ROW_ID != (this->id = this->db->insert(table,this->columns)));

	// otherwise UPDATE only dirty columns
	std::vector<ColumnBase *> cols;
	for (std::vector<ColumnBase *>::iterator iter = this->columns.begin(); iter != this->columns.end(); iter++)
	{
		if ((*iter)->isDirty())
			cols.push_back(*iter);
	}

	std::wstringstream cond;
	cond << L"id=" << this->id;
	bool r = this->db->update(table,cols,cond.str());
	if (r)
	{
		for (std::vector<ColumnBase *>::iterator iter = cols.begin(); iter != cols.end(); iter++)
		{
			(*iter)->setDirty(false);
		}
	}

	return r;
}

bool 
Entity::del()
{
	std::wstringstream cond;
	cond << L"id=" << this->id;
	return this->db->remove(table,cond.str());
}