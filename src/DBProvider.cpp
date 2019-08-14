#include "DBProvider/DBProvider.h"
#include "Shared/ParsingTools.h"
#include "Shared/Logger.h"
#include "Shared/TextTable.h"
#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <string>
#include <iostream>

using namespace std;
using namespace DBConnection;

DBProvider::DBProvider(string loginStringPG)
{
	try
	{
		currentConnection = make_shared<Connection>(loginStringPG);
		currentConnection->setConnection(loginStringPG);
	}
	catch (const exception& err)
	{
		cerr << "Wrong Parameters: " << loginStringPG << endl;
		throw err;
	}
}

vector<ObjectData> DBProvider::getObjects() const
{
	// example:
	// output - public, myFunction, function, <param1, param2, param3>
	//          common, myTable,    table,    <>
	string sql_getObjects = "SELECT /*sequences */"
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

	const auto queryResult = query(sql_getObjects);
	vector<ObjectData> objects;

	for (pqxx::result::const_iterator row = queryResult.begin(); row != queryResult.end(); ++row)
	{
		const vector<string> parameters;
		objects.push_back(ObjectData(row["obj_name"].as<string>(),
									 row["obj_type"].as<string>(),
									 row["obj_schema"].as<string>(), parameters));
	}

	return objects;
}

ScriptData DBProvider::getScriptData(const ObjectData& data) const // Temporary only for tables
{
	ObjectInformation info = getObjectInformation(data); // Getting information about object

	// "CREATE TABLE" block - initialization of all table's columns
	string scriptString = string("CREATE TABLE ") + data.scheme + "." + data.name + " (";
	for (const Column& column : info.columns)
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

	// "COMMENT ON TABLE" block
	if (!info.description.empty())
	{
		scriptString += "COMMENT ON TABLE " + data.scheme + "." + data.name + " IS '" + info.description + "';\n\n";
	}

	// "COMMENT ON COLUMN blocks
	for (const Column& column : info.columns)
	{
		if (!column.description.empty())
		{
			scriptString += "COMMENT ON COLUMN " + data.scheme + "." + data.name + "." + column.name + " IS '" + column.description + "';\n\n";
		}
	}

	// Triggers creation
	for (const Trigger& trigger : info.triggers)
	{
		scriptString += "CREATE TRIGGER " + trigger.name + "\n";
		scriptString += trigger.timing + " " + trigger.manipulation + "\n";
		scriptString += "ON " + data.scheme + "." + data.name + "\n";
		scriptString += "FOR EACH " + trigger.orientation + "\n";
		scriptString += trigger.action + ";\n\n";
	}

	ScriptData scriptData = ScriptData(data, scriptString);
	scriptData.name += ".sql";
	return scriptData;
}

// Checks if specified object exists in database
bool DBProvider::doesCurrentObjectExists(const string& scheme, const string& signature, const string& type) const
{
	bool res = false;
	if (type == "table")
	{
		res = tableExists(scheme, signature);
	}

	if (type == "sequence")
	{
		res = sequenceExists(scheme, signature);
	}

	if (type == "view")
	{
		res = viewExists(scheme, signature);
	}

	if (type == "trigger")
	{
		res = triggerExists(scheme, signature);
	}

	if (type == "function")
	{
		res = functionExists(signature, signature);
	}

	if (type == "index")
	{
		res = indexExists(signature, signature);
	}

	return res;
}

pqxx::result DBProvider::query(const string stringSQL) const
{
	auto temp = currentConnection->getParameters().loginStringPqxx();

	pqxx::work trans(*currentConnection->getConnection(), "trans");

	// Get result from database
	pqxx::result res = trans.exec(stringSQL);
	trans.commit();
	return res;
}

bool DBProvider::tableExists(const string& schema, const string& tableName) const
{
	// Define SQL request
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.tables "
			"WHERE table_schema = '${}' "
			"AND table_name = '${}');",
			vector<string> { schema, tableName });

	const auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::sequenceExists(const string& schema, const string& sequenceName) const
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.sequences "
			"WHERE sequence_schema = '${}' "
			"AND sequence_name = '${}');",
			vector<string> { schema, sequenceName });

	const auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::indexExists(const string& schema, const string& indexName) const
{
	string q =
		ParsingTools::interpolateAll("SELECT EXISTS (SELECT * FROM pg_indexes WHERE schemaname = '${}' and indexname = '${}');",
									 vector<string> { schema, indexName });

	const auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::functionExists(const string& schema, const string& funcSignature) const
{
	string q =
		ParsingTools::interpolateAll("SELECT EXISTS (SELECT * FROM information_schema.routines r, pg_catalog.pg_proc p WHERE"
									 " r.specific_schema = '${}' and r.routine_name||'('||COALESCE(array_to_string(p.proargnames, ',', '*'),'')||')' = '${}'"
									 " and r.external_language = 'PLPGSQL' and r.routine_name = p.proname and"
									 " r.specific_name = p.proname || '_' || p.oid);",
									 vector<string> { schema, funcSignature });

	const auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::viewExists(const string& tableSchema, const string& tableName) const
{
	string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.views "
			"WHERE table_schema = '${}' "
			"AND table_name = '${}');",
			vector<string> { tableSchema, tableName });

	const auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

bool DBProvider::triggerExists(const string& triggerSchema, const string& triggerName) const
{
	const string q =
		ParsingTools::interpolateAll(
			"SELECT EXISTS (SELECT * "
			"FROM information_schema.triggers "
			"WHERE trigger_schema = '${}' "
			"AND trigger_name = '${}');",
			vector<string> { triggerSchema, triggerName });

	const auto queryResult = query(q);
	return queryResult.begin()["exists"].as<bool>();
}

ObjectInformation DBProvider::getObjectInformation(const ObjectData& data) const
{
	ObjectInformation info;

	// Getting columns information
	string queryString = "SELECT * FROM information_schema.columns c JOIN (SELECT a.attname, format_type(a.atttypid, a.atttypmod) "
		"FROM pg_attribute a JOIN pg_class b ON a.attrelid = b.relfilenode "
		"WHERE a.attnum > 0 "
		"AND NOT a.attisdropped "
		"AND b.oid = '" + data.scheme + "." + data.name + "'::regclass::oid) i "
		"ON c.column_name = i.attname "
		"JOIN (SELECT t.table_schema, t.table_name, t.column_name, pgd.description "
		"FROM pg_catalog.pg_statio_all_tables as st "
		"INNER JOIN pg_catalog.pg_description pgd on(pgd.objoid = st.relid) "
		"INNER JOIN information_schema.columns t on(pgd.objsubid = t.ordinal_position "
		"AND  t.table_schema = st.schemaname and t.table_name = st.relname) "
		"WHERE t.table_catalog = '" + currentConnection->getParameters().database + "' "
		"AND t.table_schema = '" + data.scheme + "' "
		"AND t.table_name = '" + data.name + "') j "
		"ON c.column_name = j.column_name "
		"WHERE c.table_schema = '" + data.scheme + "' AND c.table_name = '" + data.name + "';";
	pqxx::result result = query(queryString); // SQL query result, contains information in table format
	for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row)
	{
		Column column;
		column.name = row["column_name"].c_str();
		column.type = row["format_type"].c_str();
		column.defaultValue = row["column_default"].c_str();
		column.description = row["description"].c_str();
		column.setNullable(row["is_nullable"].c_str());

		info.columns.push_back(column);
	}

	// Getting table owner
	queryString = "SELECT * FROM pg_tables t where schemaname = '" + data.scheme + "' and tablename = '" + data.name + "'";
	info.owner = getSingleValue(queryString, "tableowner");

	// Getting table description
	queryString = "SELECT obj_description('" + data.scheme + "." + data.name + "'::regclass::oid)";
	info.description = getSingleValue(queryString, "obj_description");

	// Getting triggers
	queryString = "SELECT * FROM information_schema.triggers t WHERE t.trigger_schema = '"
		+ data.scheme + "' and t.event_object_table = '" + data.name + "'";
	result = query(queryString);
	for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row)
	{
		Trigger* trigger = info.getTrigger(row["trigger_name"].c_str());
		if (trigger == nullptr)
		{
			trigger = new Trigger();
			trigger->name = row["trigger_name"].c_str();
			trigger->timing = row["action_timing"].c_str();
			trigger->manipulation = row["event_manipulation"].c_str();
			trigger->action = row["action_statement"].c_str();
			info.triggers.push_back(*trigger);
		}
		else
		{
			trigger->manipulation += " OR ";
			trigger->manipulation += row["event_manipulation"].c_str();
		}
	}

	return info;
}

inline string DBProvider::getSingleValue(const string& queryString, const string& columnName) const
{
	const pqxx::result result = query(queryString);
	const pqxx::result::const_iterator row = result.begin();
	return row[columnName].c_str();
}

void printObjectsData(const pqxx::result queryResult)
{
	// Iterate over the rows in our result set.
	// Result objects are containers similar to vector and such.
	for (pqxx::result::const_iterator row = queryResult.begin();
		 row != queryResult.end();
		 ++row)
	{
		cout
			<< row["obj_schema"].as<string>() << "\t"
			<< row["obj_name"].as<string>() << "\t"
			<< row["obj_type"].as<string>()
			<< endl;
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
