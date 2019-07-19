#include <DBProvider/DBProvider.h>
#include <pqxx/pqxx>
#include <pqxx/transaction>

DBProvider::DBProvider(std::string args)
{
	conn = new DBConnection(args);
}

DBProvider::~DBProvider()
{
	delete conn;
}

std::vector<std::tuple<Schema, ObjectName, ObjectType>> DBProvider::getObjects()
{
	return std::vector<std::tuple<Schema, ObjectName, ObjectType>>();
}
