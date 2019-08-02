#include "DBProvider/DBProvider.h"
#include "Shared/TextTable.h"

#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <string>
#include <iostream>

using namespace std;

DBProvider::DBProvider(const std::string args)
{
	_connection = new DBConnection(args);
	_connection->setConnection();
}

DBProvider::~DBProvider()
{
	delete _connection;
}

vector<ObjectData> DBProvider::getObjects() const
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
	std::vector<ObjectData> objects;

	for (pqxx::result::const_iterator row = resOfQuery.begin(); row != resOfQuery.end(); ++row)
	{
		const std::vector<std::string> parameters;
		objects.push_back(ObjectData(	row["obj_name"].as<std::string>(),
										row["obj_type"].as<std::string>(),
										row["obj_schema"].as<std::string>(), parameters));
	}

	return objects;
}

ScriptData DBProvider::getScriptData(const ObjectData &data) const // Temporary only for tables
{
	// Getting information from database

	ObjectInformation info; // Here will be all information about databse object

	// Getting columns information not including type
	string queryString = "SELECT * FROM information_schema.columns c JOIN (SELECT a.attname, format_type(a.atttypid, a.atttypmod) "
		"FROM pg_attribute a JOIN pg_class b ON a.attrelid = b.relfilenode "
		"WHERE a.attnum > 0 "
		"AND NOT a.attisdropped "
		"AND b.oid = '" + data.scheme + "." + data.name + "'::regclass::oid) i "
		"ON c.column_name = i.attname "
		"WHERE c.table_schema = '" + data.scheme + "' AND table_name = '" + data.name + "';";
	pqxx::result result = query(queryString); // SQL query result, contains information in table format
	for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row)
	{
		Column column;
		column.name = row["column_name"].c_str();
		column.type = row["format_type"].c_str();
		column.defaultValue = row["column_default"].c_str();
		column.setNullable(row["is_nullable"].c_str());

		info.columns.push_back(column);
	}

	// Getting table owner
	queryString = string("SELECT * FROM pg_tables t where schemaname = '") 
	+ data.scheme + "' and tablename = '" + data.name + "'";
	result = query(queryString);
	const pqxx::result::const_iterator row = result.begin();
	info.owner = row["tableowner"].c_str();


	// Script text creating.
	// Collect the text from parts of ObjectInformation

	// "CREATE TABLE" block - initialization of all table's columns
	string scriptString = string("CREATE TABLE ") + data.scheme + "." + data.name + " (";
	for (const Column &column : info.columns)
	{
		scriptString += "\n" + column.name + " " + column.type;
		if (!column.defaultValue.empty())
		{
			scriptString += " DEFAULT " + column.defaultValue;
		}
		if (!column.isNullable())
		{
			scriptString += " NOT NULL";
		}
		scriptString += ",";
	}
	if (!info.columns.empty())
	{
		scriptString.pop_back(); // Removing an extra comma at the end
	}
	scriptString += "\n);\n\n";

	// "OWNER TO" block to make the owner user
	scriptString += "ALTER TABLE " + data.scheme + "." + data.name + " OWNER TO " + info.owner + ";\n\n";

	return ScriptData(data, scriptString);
}

// Checks if specified object exists in database
 bool DBProvider::doesCurrentObjectExists(const std::string scheme, const std::string name, const std::string type) const
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

pqxx::result DBProvider::query(const std::string stringSQL) const
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

bool DBProvider::tableExists(const std::string& tableSchema, const std::string& tableName) const
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

bool DBProvider::sequenceExists(const std::string& sequenceSchema, const std::string& sequenceName) const
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

bool DBProvider::viewExists(const std::string& tableSchema, const std::string& tableName) const
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

bool DBProvider::triggerExists(const std::string& triggerSchema, const std::string& triggerName) const
{
	const string q =
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

bool Column::isNullable() const
{
	return this->nullable_;
}

void Column::setNullable(string value)
{
	transform(value.begin(), value.end(), value.begin(), tolower);
	if (value == "yes")
	{
		this->nullable_ = true;
	}
	else
	{
		this->nullable_ = false;
	}
}
