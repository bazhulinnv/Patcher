#include "DBProvider/DBProvider.h"
#include "Shared/TextTable.h"

#include <pqxx/pqxx>
#include <pqxx/transaction>
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
	std::string getObjects = "SELECT /*sequences */"
		"f.sequence_schema AS obj_schema,"
		"f.sequence_name AS obj_name,"
		"'sequence' AS obj_type "
		"FROM information_schema.sequences f "
		"UNION ALL "
		"SELECT /*tables */ "
		"f.table_schema AS obj_schema,"
		"f.table_name AS obj_name,"
		"'tables' AS obj_type "
		"FROM information_schema.tables f "
		"WHERE f.table_schema in"
		"('public', 'io', 'common', 'secure');";;

	auto resOfQuery = query(getObjects);
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

void DBProvider::printObjectsData()
{
	// Get all objects from database
	vector<ObjectData> objs = getObjects();

	// Print objs using TextTable
	TextTable resultOutput('-', '|', '+');
	resultOutput.add("NAME");
	resultOutput.add("TYPE");
	resultOutput.add("SCHEME");
	resultOutput.add("PARAMS");
	resultOutput.endOfRow();

	for (auto i = 0; i < objs.size(); ++i)
	{
		resultOutput.add(objs[i].name);
		resultOutput.add(objs[i].type);
		resultOutput.add(objs[i].scheme);

		std::string acc = "";
		if (!objs[i].paramsVector.empty())
		{
			for (const auto& param : objs[i].paramsVector)
			{
				acc += param + " ";
			}
		}

		resultOutput.add(acc);
		resultOutput.endOfRow();
	}

	resultOutput.setAlignment(2, TextTable::Alignment::RIGHT);
	std::cout << resultOutput;
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
