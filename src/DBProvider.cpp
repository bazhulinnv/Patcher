#include <DBProvider/DBProvider.h>
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
	std::string getObjects =
		"SELECT /*sequences */\
				f.sequence_schema AS obj_schema, f.sequence_name AS obj_name, 'sequence' AS obj_type\
				from information_schema.sequences f\
			UNION ALL\
			SELECT /*tables */\
				f.table_schema AS obj_schema, f.table_name AS obj_name, 'tables' AS obj_type\
				from information_schema.tables f\
			WHERE f.table_schema in('public', 'io', 'common', 'secure')";

	auto resOfQuery = query(getObjects);
	pqxx::result::const_iterator row;
	std::vector<ObjectData> objects;

	for (row = resOfQuery.begin();
		row != resOfQuery.end();
		++row)
	{
		std::vector<std::string> parameters;
		objects.push_back(ObjectData(row["obj_name"].as<std::string>(), row["obj_type"].as<std::string>(), row["obj_schema"].as<std::string>(), parameters));
	}
	return objects;
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
