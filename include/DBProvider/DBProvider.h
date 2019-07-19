#ifndef DBPROVIDER_H
#define DBPROVIDER_H

#include "../include/DBProvider/DBConnection.h"
#include <tuple>
#include <pqxx/pqxx>

typedef int Schema;
typedef int ObjectName;
typedef int ObjectType;

struct StringSQLCollection
{
	std::string getObjects =
			"SELECT /*sequences */\
				f.sequence_schema AS obj_schema, f.sequence_name AS obj_name, 'sequence' AS obj_type\
				from information_schema.sequences f\
			UNION ALL\
			SELECT /*tables */\
				f.table_schema AS obj_schema, f.table_name AS obj_name, 'tables' AS obj_type\
				from information_schema.tables f\
			WHERE f.table_schema in('public', 'io', 'common', 'secure')";
};

class DBProvider
{
public:
	explicit DBProvider(std::string args);
	~DBProvider();
	std::vector<std::tuple<Schema, ObjectName, ObjectType>> getObjects();
	pqxx::result query(std::string strSQL);
	void printQueryData(pqxx::result res);

private:
	DBConnection *conn = nullptr;
};

#endif