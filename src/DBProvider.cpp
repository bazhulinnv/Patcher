#include "DBProvider/DBProvider.h"
#include "Shared/TextTable.h"

#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <string>
#include <iostream>

using namespace std;

DBProvider::DBProvider(std::string args)
{
	_connection = new DBConnection(args);
	_connection->setConnection();
}

DBProvider::~DBProvider()
{
	delete _connection;
}

vector<ObjectData> DBProvider::getObjects()
{
	// example:
	// output - public, myFunction, function, <param1, param2, param3>
	//          common, myTable,    table,    <>
	std::string sql_getObjects = "SELECT /*sequences */"
		"f.sequence_schema AS obj_schema,"
		"f.sequence_name AS obj_name,"
		"'sequence' AS obj_type "
		"FROM information_schema.sequences f "
		"UNION ALL "
		"SELECT /*tables */ "
		"f.table_schema AS obj_schema,"
		"f.table_name AS obj_name,"
		"'table' AS obj_type "
		"FROM information_schema.tables f "
		"WHERE f.table_schema in"
		"('public', 'io', 'common', 'secure');";;

	auto resOfQuery = query(sql_getObjects);
	pqxx::result::const_iterator row;
	std::vector<ObjectData> objects;

	for (row = resOfQuery.begin(); row != resOfQuery.end(); ++row)
	{
		std::vector<std::string> parameters;
		objects.push_back(ObjectData(	row["obj_name"].as<std::string>(),
										row["obj_type"].as<std::string>(),
										row["obj_schema"].as<std::string>(), parameters));
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


// Checks if specified object exists in database
 bool DBProvider::doesCurrentObjectExists(std::string scheme, std::string name, std::string type)
{
	bool res = false;
	if (type == "table")
	{
		res = tableExists(scheme, name);
	}

	if (type == "sequence")
	{
		res = sequenceExists(scheme, name);
	}

	if (type == "view")
	{
		res = viewExists(scheme, name);
	}

	if (type == "trigger")
	{
		res = triggerExists(scheme, name);
	}

	if (type == "function")
	{
		res = functionExists(name);
	}

	if (type == "index")
	{
		res = indexExists(name);
	}

	return res;
}

pqxx::result DBProvider::query(std::string stringSQL)
{	
	// Connection must be already set
	if (!_connection->getConnection())
	{
		throw new std::exception("ERROR: Connection was not set.\n");
	}

	pqxx::work trans(*_connection->getConnection(), "trans");

	// Get result from database
	pqxx::result res = trans.exec(stringSQL);
	trans.commit();
	return res;
}

bool DBProvider::tableExists(const std::string& tableSchema, const std::string& tableName)
{
	// Define SQL request
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.tables "
			"WHERE table_schema = '${}' "
			"AND table_name = '${}');",
			vector<string> { tableSchema, tableName });

	auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::sequenceExists(const std::string& sequenceSchema, const std::string& sequenceName)
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.sequences "
			"WHERE sequence_schema = '${}' "
			"AND sequence_name = '${}');",
			vector<string> { sequenceSchema, sequenceName });

	auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::functionExists(const std::string& name)
{
	return true;
}

bool DBProvider::indexExists(const std::string& name)
{
	return true;
}

bool DBProvider::viewExists(const std::string& tableSchema, const std::string& tableName)
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.views "
			"WHERE table_schema = '${}' "
			"AND table_name = '${}');",
			vector<string> { tableSchema, tableName });

	auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::triggerExists(const std::string& triggerSchema, const std::string& triggerName)
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.triggers "
			"WHERE trigger_schema = '${}' "
			"AND trigger_name = '${}');",
			vector<string> { triggerSchema, triggerName });

	auto resOfQuery = query(q);
	return false;
}


void printObjectsData(pqxx::result queryResult)
{
	// Iterate over the rows in our result set.
	// Result objects are containers similar to std::vector and such.
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
