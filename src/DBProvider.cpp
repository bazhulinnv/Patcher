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

ScriptData DBProvider::getScriptData(const ObjectData &data) // Temporary only for tables
{
	if (data.type == "table")
	{
		return getTableData(data);
	}
	else if (data.type == "function")
	{
		return getFunctionData(data);
	}
	else if (data.type == "trigger")
	{
		return getTriggerData(data);
	}
	else if (data.type == "index")
	{
		return getIndexData(data);
	}
	else if (data.type == "table")
	{
		return getViewData(data);
	}
	else if (data.type == "view")
	{
		return getViewData(data);
	}
	else if (data.type == "sequence")
	{
		return getSequenceData(data);
	}
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

Table DBProvider::getTable(const ObjectData & data)
{
	Table table;
	// If table is partision of some other table
	// need to know only parent
	if (!initializePartitionTable(table, data))
	{
		initializeType(table, data);
		initializeOwner(table, data);
		initializeDescription(table, data);
		initializeOptions(table, data);
		initializeColumns(table, data);
		initializeSpace(table, data);
		initializeConstraints(table, data);
		initializePartitionExpression(table, data);
		initializeInheritTables(table, data);
	}
	return table;
}

string DBProvider::getSingleValue(const string &queryString, const string &columnName) const
{
	pqxx::result result = query(queryString);
	pqxx::result::const_iterator row = result.begin();
	return row[columnName].c_str();
}

ScriptData DBProvider::getTableData(const ObjectData & data)
{
	Table table = getTable(data); // Getting information about object
	string scriptString;

	// If table is not partission of some other table
	if (!table.isPartition())
	{
		// "CREATE TABLE" block - initialization of all table's columns
		scriptString = "CREATE ";
		if (table.type != "BASE TABLE")
		{
			scriptString += table.type = " ";
		}
		scriptString += "TABLE " + data.schema + "." + data.name + " (";
		for (const Column &column : table.columns)
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

		// Creation of constraints
		for (Constraint constraint : table.constraints)
		{
			scriptString += "\nCONSTRAINT " + constraint.name + " " + constraint.type + " ";
			if (constraint.type == "PRIMARY KEY" || constraint.type == "UNIQUE")
			{
				scriptString += "(" + constraint.columnName + ")";
			}
			else if (constraint.type == "FOREIGN KEY")
			{
				scriptString += "(" + constraint.columnName + ")\n";
				scriptString += "REFERENCES " + constraint.foreignTableSchema + "." + constraint.foreignTableName +
					" (" + constraint.foreignColumnName + ") ";
				if (constraint.matchOption == "NONE")
				{
					scriptString += "MATCH SIMPLE";
				}
				else
				{
					scriptString += constraint.matchOption;
				}
				scriptString += "\nON UPDATE " + constraint.onUpdate;
				scriptString += "\nON DELETE " + constraint.onDelete;
			}
			else if (constraint.type == "CHECK")
			{
				scriptString += constraint.checkClause;
			}
			scriptString += ",";
		}

		if (!table.columns.empty())
		{
			scriptString.pop_back(); // Removing an extra comma at the end
		}
		scriptString += "\n)\n";

		// "INHERITS" block
		if (!table.inheritTables.empty())
		{
			scriptString += "INHERITS (\n";
			scriptString += table.inheritTables[0];
			for (int inheritIndex = 1; inheritIndex < table.inheritTables.size(); inheritIndex++)
			{
				scriptString += ",\n" + table.inheritTables[inheritIndex];
			}
			scriptString += "\n)\n";
		}

		// "PARTITION BY" block
		if (!table.partitionExpression.empty())
		{
			scriptString += "PARTITION BY " + table.partitionExpression + "\n";
		}

		// "WITH" block to create storage parameters
		scriptString += "WITH (\n" + table.options + "\n)\n";

		// "TABLESPACE" definition
		scriptString += "TABLESPACE ";
		if (table.space.empty())
		{
			scriptString += "pg_default";
		}
		else
		{
			scriptString += table.space;
		}
		scriptString += ";\n\n";

		// "OWNER TO" block to make the owner user
		scriptString += "ALTER TABLE " + data.schema + "." + data.name + " OWNER TO " + table.owner + ";\n\n";

		// "COMMENT ON TABLE" block
		if (!table.description.empty())
		{
			scriptString += "COMMENT ON TABLE " + data.schema + "." + data.name + "\nIS '" + table.description + "';\n\n";
		}

		// "COMMENT ON COLUMN blocks
		for (const Column &column : table.columns)
		{
			if (!column.description.empty())
			{
				scriptString += "COMMENT ON COLUMN " + data.schema + "." + data.name + "." + column.name + "\nIS '" + column.description + "';\n\n";
			}
		}
	}
	else
	{
		PartittionTable parentTable = table.getPartitionTable();
		scriptString = "CREATE TABLE " + data.schema + "." + data.name +
			" PARTITION OF " + parentTable.schema + "." + parentTable.name + "\n" +
			parentTable.partitionExpression + ";\n";
	}

	ScriptData scriptData = ScriptData(data, scriptString);
	scriptData.name += ".sql";
	return scriptData;
}

ScriptData DBProvider::getFunctionData(const ObjectData & data) const
{
	return ScriptData();
}

ScriptData DBProvider::getViewData(const ObjectData & data) const
{
	return ScriptData();
}

ScriptData DBProvider::getSequenceData(const ObjectData & data) const
{
	return ScriptData();
}

ScriptData DBProvider::getTriggerData(const ObjectData & data) const
{
	return ScriptData();
}

ScriptData DBProvider::getIndexData(const ObjectData & data) const
{
	return ScriptData();
}

bool DBProvider::initializePartitionTable(Table & table, const ObjectData & data)
{
	string queryString = ""
		"WITH recursive inh AS "
		"( "
		"SELECT i.inhrelid, i.inhparent, nsp.nspname AS parent_schema "
		"FROM pg_catalog.pg_inherits i "
		"JOIN pg_catalog.pg_class cl ON i.inhparent = cl.oid "
		"JOIN pg_catalog.pg_namespace nsp ON cl.relnamespace = nsp.oid "
		"UNION ALL "
		"SELECT i.inhrelid, i.inhparent, inh.parent_schema "
		"FROM inh "
		"JOIN pg_catalog.pg_inherits i ON (inh.inhrelid = i.inhparent) "
		") "
		"SELECT c.relname AS partition_name, "
		"n.nspname AS partition_schema, "
		"c.relispartition AS is_partition, "
		"pg_get_expr(c.relpartbound, c.oid, true) AS partition_expression, "
		"pg_get_expr(p.partexprs, c.oid, true) AS sub_partition, "
		"inh.parent_schema,"
		"pg_catalog.textin(pg_catalog.regclassout(inhparent)) AS parent_name, "
		"CASE p.partstrat "
		"WHEN 'l' THEN 'LIST' "
		"WHEN 'r' THEN 'RANGE' "
		"END AS sub_partition_strategy "
		"FROM inh "
		"JOIN pg_catalog.pg_class c ON inh.inhrelid = c.oid "
		"JOIN pg_catalog.pg_namespace n ON c.relnamespace = n.oid "
		"LEFT JOIN pg_partitioned_table p ON p.partrelid = c.oid "
		"WHERE  1 = 1 "
		"AND c.relname = '" + data.name + "' "
		"AND n.nspname = '" + data.schema + "' "
		"AND c.relispartition = 'true'";
	pqxx::result result = query(queryString);

	if (result.size() == 0)
	{
		return false;
	}

	pqxx::result::const_iterator row = result.begin();
	string parentSchema = row["parent_schema"].c_str();
	string parentName = row["parent_name"].c_str();
	string partitionExpression = row["partition_expression"].c_str();
	table.setPartitionTable(parentSchema, parentName, partitionExpression);

	return true;
}

void DBProvider::initializeType(Table & table, const ObjectData & data)
{
	string queryString = "SELECT * FROM information_schema.tables t "
		"WHERE t.table_schema = '" + data.schema + "' AND t.table_name = '" + data.name + "'";
	table.type = getSingleValue(queryString, "table_type");
}

void DBProvider::initializeOwner(Table & table, const ObjectData & data)
{
	string queryString = "SELECT * FROM pg_tables t where schemaname = '" + data.schema + "' and tablename = '" + data.name + "'";
	table.owner = getSingleValue(queryString, "tableowner");
}

void DBProvider::initializeDescription(Table & table, const ObjectData & data)
{
	string queryString = "SELECT obj_description('" + data.schema + "." + data.name + "'::regclass::oid)";
	table.description = getSingleValue(queryString, "obj_description");
}

void DBProvider::initializeOptions(Table & table, const ObjectData & data)
{
	string queryString = "SELECT * FROM pg_class WHERE relname = '" + data.name + "'";
	string queryValue = getSingleValue(queryString, "reloptions");

	// Getting OIDS value
	string oidsExpression = "OIDS=false";
	if (getSingleValue(queryString, "relhasoids") == "t")
	{
		oidsExpression = "OIDS=true";
	}
	table.options += oidsExpression;

	// String feed in the required format
	vector<string> expressionString;
	if (!queryValue.empty())
	{
		queryValue.erase(0, 1); // Remove { symbol from beginning
		queryValue.pop_back(); // Remove } symbol from ending
		expressionString = ParsingTools::splitToVector(queryValue, ",");
		for (int expressionIndex = 0; expressionIndex < expressionString.size(); expressionIndex++)
		{
			table.options += ",\n" + expressionString[expressionIndex];
		}
	}

}

void DBProvider::initializeSpace(Table & table, const ObjectData & data)
{
	string queryString = "SELECT * FROM pg_tables WHERE tablename = '" + data.name + "' AND schemaname = '" + data.schema + "'";
	table.space = getSingleValue(queryString, "tablespace");
}

void DBProvider::initializeColumns(Table & table, const ObjectData & data)
{
	string queryString = "select c.table_catalog, "
		"c.table_schema, "
		"c.table_name, "
		"c.column_name, "
		"c.is_nullable, "
		"format_type(pa.atttypid, pa.atttypmod) as collumn_type, "
		"c.column_default, "
		"pd.description "
		"from information_schema.columns c "
		"join pg_class pc on(pc.oid = (c.table_schema || '.' || c.table_name) ::regclass::oid) "
		"join pg_attribute pa on(pa.attrelid = pc.oid and pa.attname = c.column_name) "
		"left join pg_catalog.pg_description pd on(pd.objoid = (c.table_schema || '.' || c.table_name)::regclass::oid and pd.objsubid = c.ordinal_position) "
		"where 1 = 1 "
		"and c.table_catalog = '" + _connection->info.databaseName + "' "
		"and c.table_schema = '" + data.schema + "' "
		"and c.table_name = '" + data.name + "'";

	pqxx::result result = query(queryString); // SQL query result, contains information in table format
	for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row)
	{
		Column column;
		column.name = row["column_name"].c_str();
		column.type = row["collumn_type"].c_str();
		column.defaultValue = row["column_default"].c_str();
		column.description = row["description"].c_str();
		column.setNullable(row["is_nullable"].c_str());

		table.columns.push_back(column);
	}
}

void DBProvider::initializePartitionExpression(Table & table, const ObjectData & data)
{
	string queryString = "SELECT c.relnamespace::regnamespace::text, c.relname, pg_get_partkeydef(c.oid) AS partition_expression "
		"FROM   pg_class c "
		"WHERE  1 = 1 "
		"AND c.relkind = 'p' "
		"AND c.relname = '" + data.name + "' "
		"AND  c.relnamespace::regnamespace::text = '" + data.schema + "'";
	pqxx::result result = query(queryString);
	if (result.size() != 0)
	{
		pqxx::result::const_iterator row = result.begin();
		table.partitionExpression = row["partition_expression"].c_str();
	}
	
}

void DBProvider::initializeConstraints(Table & table, const ObjectData & data)
{
	string queryString = "SELECT "
		"*, "
		"ccu.table_schema AS foreign_table_schema, "
		"ccu.table_name AS foreign_table_name, "
		"ccu.column_name AS foreign_column_name "
		"FROM "
		"information_schema.table_constraints AS tc "
		"LEFT JOIN information_schema.key_column_usage AS kcu "
		"ON tc.constraint_name = kcu.constraint_name "
		"AND tc.table_schema = kcu.table_schema "
		"LEFT JOIN information_schema.constraint_column_usage AS ccu "
		"ON ccu.constraint_name = tc.constraint_name "
		"AND ccu.table_schema = tc.table_schema "
		"AND tc.constraint_type = 'FOREIGN KEY' "
		"LEFT JOIN information_schema.check_constraints cc "
		"ON cc.constraint_name = tc.constraint_name "
		"LEFT JOIN information_schema.referential_constraints rc "
		"ON rc.constraint_name = tc.constraint_name "
		"WHERE tc.table_name = '" + data.name + "' "
		"AND tc.table_schema = '" + data.schema + "' "
		"AND COALESCE(cc.check_clause, '') NOT ILIKE '%IS NOT NULL%' ";
	pqxx::result result = query(queryString);
	for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row)
	{
		Constraint constraint;
		constraint.type = row["constraint_type"].c_str();
		constraint.name = row["constraint_name"].c_str();
		constraint.columnName = row["column_name"].c_str();
		constraint.checkClause = row["check_clause"].c_str();
		constraint.foreignTableSchema = row["foreign_table_schema"].c_str();
		constraint.foreignTableName = row["foreign_table_name"].c_str();
		constraint.foreignColumnName = row["foreign_column_name"].c_str();
		constraint.matchOption = row["match_option"].c_str();
		constraint.onDelete = row["delete_rule"].c_str();
		constraint.onUpdate = row["update_rule"].c_str();

		table.constraints.push_back(constraint);
	}

}

void DBProvider::initializeInheritTables(Table & table, const ObjectData & data)
{
<<<<<<< HEAD
	const pqxx::result result = query(queryString);
	const pqxx::result::const_iterator row = result.begin();
	return row[columnName].c_str();
=======
	string queryString = "SELECT "
		"nmsp_parent.nspname AS parent_schema, "
		"parent.relname      AS parent_name, "
		"nmsp_child.nspname  AS child_schema, "
		"child.relname       AS child_name "
		"FROM pg_inherits "
		"JOIN pg_class parent            ON pg_inherits.inhparent = parent.oid "
		"JOIN pg_class child             ON pg_inherits.inhrelid = child.oid "
		"JOIN pg_namespace nmsp_parent   ON nmsp_parent.oid = parent.relnamespace "
		"JOIN pg_namespace nmsp_child    ON nmsp_child.oid = child.relnamespace "
		"WHERE child.relispartition = 'false' "
		"AND child.relname = '" + data.name + "' "
		"AND nmsp_child.nspname = '" + data.schema + "'";
	pqxx::result result = query(queryString);
	for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row)
	{
		table.inheritTables.push_back(row["parent_name"].c_str());
	}
>>>>>>> 37b3b2fcdb3fa83a69161a5c1195fbd5caacdf7a
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

void Table::setPartitionTable(string schema, string name, string partitionExpression)
{
	this->_partitionTable.name = name;
	this->_partitionTable.schema = schema;
	this->_partitionTable.partitionExpression = partitionExpression;
	this->_isPartition = true;
}

PartittionTable Table::getPartitionTable()
{
	return _partitionTable;
}

bool Table::isPartition()
{
	return _isPartition;
}
