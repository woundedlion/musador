#ifndef RESULTSET_5ED0E0D1_ED9A_4de0_931B_0448E601D090
#define RESULTSET_5ED0E0D1_ED9A_4de0_931B_0448E601D090

#include <vector>
#include "boost/shared_ptr.hpp"
//#include "Database.h"

class Database;

class ResultSet
{
public:
	
	ResultSet(boost::shared_ptr<Database> db);

	virtual ~ResultSet() {};

	boost::shared_ptr<Database> getDb();

	virtual unsigned long count() = 0;
	
	virtual bool next() = 0;
	
	virtual void reset() = 0;

	virtual int getSize(int iCol) const = 0;

	virtual const unsigned char * getBlob(int iCol) const = 0;
	
	virtual double getDouble(int iCol) const = 0;
	
	virtual int getInt(int iCol) const = 0;
	
	virtual __int64 getInt64(int iCol) const = 0;
	
	virtual const char * getText(int iCol) const = 0;
	
private:

	boost::shared_ptr<Database> db;
};

#endif