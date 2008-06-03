#include "ResultSet.h"

using namespace Musador::Database;

ResultSet::ResultSet(boost::shared_ptr<IDatabase> dbPtr) :
db(dbPtr)
{
}

boost::shared_ptr<IDatabase> 
ResultSet::getDb()
{
	return this->db;
}