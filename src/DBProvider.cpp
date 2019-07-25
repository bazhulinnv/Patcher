#include "DBProvider/DBProvider.h"

#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <iostream>

using namespace std;

DBProvider::DBProvider(std::string args)
{
	conn = new DBConnection(args);
	conn->setConnection();
}

DBProvider::~DBProvider()
{
	delete conn;
}

vector<ObjectData> DBProvider::getObjects()
{
	// example:
	// output - public, myFunction, function, <param1, param2, param3>
	//          common, myTable,    table,    <>
	return vector<ObjectData>();
}

ScriptData DBProvider::getScriptData(ObjectData)
{
	// example:
	// input  - public, myFunction,     function, <param1, param2, param3>
	// output - public, myFunction.sql, function, <param1, param2, param3>, some script text
	return ScriptData();
}

pqxx::result DBProvider::query(std::string stringSQL)
{	
	// Connection must be already set
	if (!conn->getConnection())
	{
		throw new std::exception("ERROR: Connection was not set.\n");
	}

	pqxx::work trans(*conn->getConnection(), "trans");

	// Get result from database
	pqxx::result res = trans.exec(stringSQL);
	trans.commit();
	return res;
}

void printObjectsData(pqxx::result queryResult)
{
	// Iterate over the rows in our result set.
	// Results objects are containers similar to std::vector and such.
	for (	pqxx::result::const_iterator row = queryResult.begin();
			row != queryResult.end();
			++row )
	{
		std::cout
			<< row["obj_schema"].as<std::string>() << "\t"
			<< row["obj_name"].as<std::string>() << "\t"
			<< row["obj_type"].as<std::string>()
			<< std::endl;
	}
}
