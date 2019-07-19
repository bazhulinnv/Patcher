#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "../include/DBProvider/DBConnection.h"
#include <tuple>
#include <pqxx/pqxx>

typedef int Schema;
typedef int ObjectName;
typedef int ObjectType;

class DBProvider
{
public:
	explicit DBProvider(std::string args);
	~DBProvider();
	std::vector<std::tuple<Schema, ObjectName, ObjectType>> getObjects();
	// pqxx::result query(std::string strSQL);

private:
	DBConnection *conn = nullptr;
};

#endif