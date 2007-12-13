#ifndef ENTITY_2CC11132_9780_4998_A779_3327320D5037
#define ENTITY_2CC11132_9780_4998_A779_3327320D5037

#include <vector>
#include <boost/smart_ptr.hpp>
#include "Database.h"


class Entity
{
public:

	friend class DatabaseTest;

	Entity(boost::shared_ptr<Database> dbPtr, std::wstring table);
	virtual ~Entity() = 0;

	inline unsigned long getId() { return this->id; }
	virtual bool load(unsigned long id);
	virtual bool save();
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

template <class T> 
struct EntitySet
{
	typedef std::auto_ptr<std::vector<boost::shared_ptr<Entity> > > Type;
};


#endif