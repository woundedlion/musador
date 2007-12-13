#include "ResultSet.h"

ResultSet::ResultSet(boost::shared_ptr<Database> dbPtr) :
db(dbPtr)
{
}

boost::shared_ptr<Database> ResultSet::getDb()
{
	return this->db;
}