#ifndef RESULTSETSQLITE_5ED0E0D1_ED9A_4de0_931B_0448E601D090
#define RESULTSETSQLITE_5ED0E0D1_ED9A_4de0_931B_0448E601D090

#include "ResultSet.h"

struct sqlite3_stmt;

class ResultSetSqlite : public ResultSet
{
public:

	ResultSetSqlite(boost::shared_ptr<Database> db, sqlite3_stmt * stmt);
	~ResultSetSqlite();

	unsigned long count();
	bool next();
	void reset();

	int getSize(int iCol) const;

	const unsigned char * getBlob(int iCol) const;
	double getDouble(int iCol) const;
	int getInt(int iCol) const;
	__int64 getInt64(int iCol) const;
	const char * getText(int iCol) const;

private:

	sqlite3_stmt * stmt;

};

#endif