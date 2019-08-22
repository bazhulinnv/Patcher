#include "DBProvider/DBProvider.h"
#include "Shared/ParsingTools.h"
#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <string>
#include <iostream>
#include <utility>

using namespace std;
using namespace DBConnection;

namespace Provider
{
	DBProvider::DBProvider(const string& login_string_pg)
	{
		try
		{
			current_connection_ = make_shared<Connection>();
			current_connection_->SetConnection(const_cast<std::string&>(login_string_pg));
		}
		catch (const exception& error)
		{
			cerr << "Wrong Parameters: " << login_string_pg << endl;
			throw error;
		}
	}

	DBProvider::DBProvider(shared_ptr<Connection> already_set_connection)
	{
		try
		{
			current_connection_ = std::move(already_set_connection);
			current_connection_->SetConnection();
		}
		catch (const exception& error)
		{
			if (already_set_connection->IsConnectionSet())
			{
				const auto params = already_set_connection->GetParameters();
				cerr << "\nPassed connection was not set properly." << endl;

				cerr << "\tHostname: ";
				params.hostname.empty() ? cerr << "<empty>" << endl : cerr << params.hostname << endl;

				cerr << "\tPort: " << params.port << endl;

				cerr << "\tDatabase: ";
				params.database.empty() ? cerr << "<empty>" << endl : cerr << params.database << endl;

				cerr << "\tUsername: ";
				params.username.empty() ? cerr << "<empty>" << endl : cerr << params.username << endl;

				cerr << "\tPassword: ";
				params.password.empty() ? cerr << "<empty>" << endl : cerr << params.password << endl;
				throw invalid_argument("[DBProvider]: Passed connection was not set properly.\n");
			}

			if (already_set_connection->IsOpen()) throw invalid_argument("[DBProvider]: Could not open passed connection.\n");

			throw error;
		}
	}

	DBProvider::~DBProvider()
	{
		current_connection_.reset();
	}

	vector<ObjectData> DBProvider::GetObjects() const
	{
		// example:
		// output - public, myFunction, function, <param1, param2, param3>
		//          common, myTable,    table,    <>
		const string sql_get_objects = "SELECT /*sequences */"
			"f.sequence_schema AS obj_schema,"
			"f.sequence_name AS obj_name,"
			"'SequenceType' AS obj_type "
			"FROM information_schema.sequences f "
			"UNION ALL "
			"SELECT /*tables */ "
			"f.table_schema AS obj_schema,"
			"f.table_name AS obj_name,"
			"'table' AS obj_type "
			"FROM information_schema.tables f "
			"WHERE f.table_schema in"
			"('public', 'io', 'common', 'secure');";

		const auto query_result = Query(sql_get_objects);
		vector<ObjectData> objects;

		for (auto row = query_result.begin(); row != query_result.end(); ++row)
		{
			auto obj_type = CastObjectType(row["obj_type"].as<string>());

			const vector<string> parameters;
			objects.emplace_back(ObjectData(row["obj_name"].as<string>(), obj_type, row["obj_schema"].as<string>(), parameters));
		}

		return objects;
	}

	ScriptDefinition DBProvider::GetScriptData(const ObjectData& data, vector<ScriptDefinition>& extra_script_data) const
	{
		switch (data.type)
		{
			case ObjectType::TableType: return GetTableData(data, extra_script_data);
			case ObjectType::FunctionType: return GetFunctionData(data);
			case ObjectType::TriggerType: return GetTriggerData(data);
			case ObjectType::ViewType: return GetViewData(data);
			case ObjectType::IndexType: return GetIndexData(data);
			case ObjectType::SequenceType: return GetSequenceData(data);
		}
	}

	// Checks if specified object exists in database
	bool DBProvider::DoesCurrentObjectExists(const string& scheme, const string& signature, const string& type) const
	{
		const ObjectType type_ = CastObjectType(type);
		switch (type_)
		{
			case ObjectType::TableType:    return TableExists(scheme, signature);
			case ObjectType::FunctionType: return FunctionExists(scheme, signature);
			case ObjectType::TriggerType:  return TriggerExists(scheme, signature);
			case ObjectType::ViewType:     return ViewExists(scheme, signature);
			case ObjectType::IndexType:    return IndexExists(scheme, signature);
			case ObjectType::SequenceType: return SequenceExists(scheme, signature);
			default: return false;
		}
	}

	pqxx::result DBProvider::Query(const string& string_PLPG_SQL) const
	{
		if (!current_connection_->IsOpen())
		{
			throw runtime_error("ERROR: Couldn't execute query. Database connection is dead.\n");
		}

		pqxx::work transaction(*current_connection_->GetConnection(), "query transaction");

		// Get result from database
		try
		{
			pqxx::result query_result = transaction.exec(string_PLPG_SQL);
			transaction.commit();
			return query_result;
		}
		catch (const pqxx::sql_error& err)
		{
			cerr << "Failed to execute query" << endl;
			cerr << "SQL error: " << err.what() << endl;
			cerr << "Query was: " << err.query() << endl;
			throw runtime_error(ParsingTools::Interpolate("ERROR: Couldn't execute query. SQL error occured.\n${}\n", err.what()));
		}
	}

	pair<bool, pqxx::result> DBProvider::QueryWithStatus(const string& string_PLPG_SQL) const
	{
		if (!current_connection_->IsOpen())
		{
			throw runtime_error("ERROR: Couldn't execute query. Database connection is dead.\n");
		}

		pqxx::work transaction(*current_connection_->GetConnection(), "query transaction");
		pqxx::result query_result;

		// Get result from database
		try
		{
			query_result = transaction.exec(string_PLPG_SQL);
			transaction.commit();
			return make_pair(true, query_result);
		}
		catch (const pqxx::sql_error& err)
		{
			return make_pair(false, query_result);
		}
	}

	void DBProvider::InsertToDB(ObjectData obj)
	{
	}

	void DBProvider::DeleteFromDB(ObjectData obj)
	{
	}

	void DBProvider::Update(ObjectData obj)
	{
	}

	ObjectDataVectorType DBProvider::GetType(ObjectData obj)
	{
		return {};
	}

	vector<ObjectData> DBProvider::UseViewToGetData(string name_of_view)
	{
		return {};
	}

	vector<ObjectData> DBProvider::CreateAndUseView(string name_of_view, string body_of_view)
	{
		return {};
	}

	bool DBProvider::TableExists(const string& schema, const string& tableName) const
	{
		// Define SQL request
		const string query_request =
			ParsingTools::InterpolateAll(
				"SELECT EXISTS (SELECT * "
				"FROM information_schema.tables "
				"WHERE table_schema = '${}' "
				"AND table_name = '${}');",
				vector<string> { schema, tableName });

		const auto query_result = Query(query_request);
		return query_result.begin()["exists"].as<bool>();
	}

	bool DBProvider::SequenceExists(const string& schema, const string& sequence_name) const
	{
		const string query_request =
			ParsingTools::InterpolateAll(
				"SELECT EXISTS (SELECT * "
				"FROM information_schema.sequences "
				"WHERE sequence_schema = '${}' "
				"AND sequence_name = '${}');",
				vector<string> { schema, sequence_name });

		const auto query_result = Query(query_request);
		return query_result.begin()["exists"].as<bool>();
	}

	bool DBProvider::IndexExists(const string& schema, const string& index_name) const
	{
		const string query_request = ParsingTools::InterpolateAll(
			"SELECT EXISTS (SELECT * FROM pg_indexes WHERE schemaname = '${}'"
			" AND indexname = '${}');", vector<string> { schema, index_name });

		const auto query_result = Query(query_request);
		return query_result.begin()["exists"].as<bool>();
	}

	bool DBProvider::FunctionExists(const string& schema, const string& func_signature) const
	{
		const string query_request =
			ParsingTools::InterpolateAll(
				"SELECT EXISTS (SELECT * FROM information_schema.routines r, pg_catalog.pg_proc p WHERE"
				" r.specific_schema = '${}' and r.routine_name||'('||COALESCE(array_to_string(p.proargnames, ',', '*'),'')||')' = '${}'"
				" and r.external_language = 'PLPGSQL' and r.routine_name = p.proname and"
				" r.specific_name = p.proname || '_' || p.oid);",
				vector<string> { schema, func_signature });

		const auto queryResult = Query(query_request);
		return queryResult.begin()["exists"].as<bool>();
	}

	bool DBProvider::ViewExists(const string& table_schema, const string& table_name) const
	{
		const string query_request =
			ParsingTools::InterpolateAll(
				"SELECT EXISTS (SELECT * "
				"FROM information_schema.views "
				"WHERE table_schema = '${}' "
				"AND table_name = '${}');",
				vector<string> { table_schema, table_name });

		const auto query_result = Query(query_request);
		return query_result.begin()["exists"].as<bool>();
	}

	bool DBProvider::TriggerExists(const string& trigger_schema, const string& trigger_name) const
	{
		const string query_request =
			ParsingTools::InterpolateAll(
				"SELECT EXISTS (SELECT * "
				"FROM information_schema.triggers "
				"WHERE trigger_schema = '${}' "
				"AND trigger_name = '${}');",
				vector<string> { trigger_schema, trigger_name });

		const auto query_result = Query(query_request);
		return query_result.begin()["exists"].as<bool>();
	}

	Table DBProvider::GetTable(const ObjectData& data) const
	{
		Table table;
		// If table is partision of some other table
		// need to know only parent
		if (!InitializePartitionTable(table, data))
		{
			InitializeType(table, data);
			InitializeOwner(table, data);
			InitializeDescription(table, data);
			InitializeOptions(table, data);
			InitializeColumns(table, data);
			InitializeSpace(table, data);
			InitializeConstraints(table, data);
			InitializePartitionExpression(table, data);
			InitializeInheritTables(table, data);
			InitializeIndexExpressions(table, data);
		}

		return table;
	}

	Function DBProvider::GetFunction(const ObjectData& data) const
	{
		string query_request =
			"SELECT * , array_to_string(p.proargnames, ', ', '*'), pg_get_functiondef(p.oid)"
			" FROM information_schema.routines r, pg_catalog.pg_proc p"
			" WHERE r.external_language = 'PLPGSQL'"
			" AND r.routine_name = p.proname"
			" AND r.routine_schema = '${}'"
			" AND r.specific_name = '${}' || '_' || p.oid"
			" AND array_to_string(p.proargnames, ', ', '*') = '${}'";

		const auto data_params = ParsingTools::JoinAsString(data.params, ", ");

		query_request = ParsingTools::InterpolateAll(query_request,
													 vector<string>{ data.schema, data.name, data_params });
		const auto query_result = Query(query_request);
		const auto row = query_result[0];

		Function function = {};
		function.specific_catalog = row["specific_catalog"].as<string>();
		function.specific_schema = row["specific_schema"].as<string>();
		function.specific_name = row["specific_name"].as<string>();
		function.routine_catalog = row["routine_catalog"].as<string>();
		function.routine_schema = row["routine_schema"].as<string>();
		function.routine_name = row["routine_name"].as<string>();
		function.routine_type = row["routine_type"].as<string>();
		function.data_type = row["data_type"].as<string>();
		function.external_language = row["external_language"].as<string>();

		vector<string> function_parameters;
		if (!row["array_to_string"].is_null()) function_parameters = ParsingTools::SplitToVector(to_string(row["array_to_string"]), ", ");

		if (!row["pg_get_functiondef"].is_null()) function.function_text_definition = row["pg_get_functiondef"].as<string>();
		auto function_signature = row["specific_name"].as<string>() + "(" + row["array_to_string"].as<string>() + ")";

		function.schema = function.routine_schema; // Schema of object
		function.name = function.routine_name; // Name of object
		function.params = function_parameters; // Params of object

		return function;
	}

	Trigger DBProvider::GetTrigger(const ObjectData& data, const string& comment, const string& code) const
	{
		const string query_request = "SELECT t.trigger_catalog, t.trigger_schema,"
			" t.trigger_name, string_agg(t.event_manipulation, ' or ')"
			" FROM information_schema.triggers t"
			" WHERE t.event_object_catalog = '" + current_connection_->GetParameters().database + "'"
			" AND t.event_object_schema = '" + data.schema + "'"
			" AND t.event_object_table = '" + data.name + "'"
			" GROUP BY t.trigger_catalog, t.trigger_schema, t.trigger_name;";

		const auto query_result = Query(query_request);
		const auto row = query_result[0];

		Trigger trigger = {};
		trigger.trigger_catalog = current_connection_->GetParameters().database;
		trigger.trigger_schema = row["trigger_schema"].as<string>();
		trigger.trigger_name = row["trigger_name"].as<string>();
		trigger.string_agg = row["string_agg"].as<string>();
		trigger.trigger_text_definition = string();

		trigger.schema = trigger.trigger_schema; // Schema of object
		trigger.name = trigger.trigger_name; // Name of object

		using namespace ParsingTools;

		trigger.trigger_text_definition =
			InterpolateAll("CREATE FUNCTION ${}.'${}'()\n	RETURNS trigger\n	LANGUAGE 'plpgsql'\n", vector<string>{trigger.schema, trigger.name}) +
			Interpolate("	NOT LEAKPROOF \n	AS $BODY${@code}$BODY$;\n\n", code, "{@code}") +
			InterpolateAll("ALTER FUNCTION ${}.'${}'()\n", vector<string>{trigger.schema, trigger.name}) +
			Interpolate("	OWNER TO ${};\n\n", current_connection_->GetParameters().username) +
			InterpolateAll("COMMENT ON FUNCTION ${}.'${}'()\n", vector<string>{trigger.schema, trigger.name}) +
			Interpolate("	IS '${}';\n", comment);

		return trigger;
	}

	Sequence DBProvider::GetSequence(const ObjectData& data, const int start_value, const int minimum_value, const int maximum_value, const int increment, string cycle_option, string comment) const
	{
		const string query_request =
			"SELECT * FROM information_schema.sequences WHERE sequence_catalog = '"
			+ current_connection_->GetParameters().database + "' AND"
			" sequence_name = '" + data.name + "' AND"
			" sequence_schema = '" + data.schema + "';";

		const pqxx::result query_result = Query(query_request);
		const auto row = query_result[0];

		Sequence sequence = {};
		sequence.sequence_catalog = row["sequence_catalog"].as<string>();
		sequence.sequence_schema = row["sequence_schema"].as<string>();
		sequence.sequence_name = row["sequence_name"].as<string>();
		sequence.data_type = row["data_type"].as<string>();

		sequence.start_value = to_string(start_value);
		sequence.minimum_value = to_string(minimum_value);
		sequence.maximum_value = to_string(maximum_value);
		sequence.increment = to_string(increment);
		sequence.cycle_option = move(cycle_option);

		sequence.schema = sequence.sequence_schema; // Schema of object
		sequence.name = sequence.sequence_name; // Name of object

		using namespace ParsingTools;

		sequence.query_create_seq =
			InterpolateAll("CREATE SEQUENCE ${}.'${}'\n	CYCLE\n", vector<string> {sequence.schema, sequence.name}) +
			Interpolate("	INCREMENT ${}\n", sequence.increment) +
			Interpolate("	START ${}\n", sequence.start_value) +
			Interpolate("	MINVALUE ${}\n", sequence.minimum_value) +
			Interpolate("	MAXVALUE ${};\n\n", sequence.maximum_value) +
			InterpolateAll("ALTER SEQUENCE ${}.'${}'\n", vector<string> {sequence.schema, sequence.name}) +
			Interpolate("	OWNER TO ${};\n\n", current_connection_->GetParameters().username) +
			InterpolateAll("COMMENT ON SEQUENCE ${}.'${}'\n", vector<string> {sequence.schema, sequence.name}) +
			Interpolate("	IS '${}';", comment);

		return sequence;
	}

	View DBProvider::GetView(const ObjectData& data) const
	{
		const string query_request =
			"SELECT * FROM information_schema.views WHERE"
			" table_catalog = '" + current_connection_->GetParameters().database + "' AND"
			" table_name = '" + data.name + "';";

		const auto query_result = Query(query_request);
		const auto row = query_result[0];

		View view = {};
		view.table_catalog = row["table_catalog"].as<string>();
		view.table_schema = row["table_schema"].as<string>();
		view.table_and_view_name = row["table_name"].as<string>();
		view.check_option = row["check_option"].as<string>();
		view.is_updatable = row["is_updatable"].as<string>();
		view.is_insertable_into = row["is_insertable_into"].as<string>();
		view.is_trigger_updatable = row["is_trigger_updatable"].as<string>();
		view.is_trigger_deletable = row["is_trigger_deletable"].as<string>();
		view.is_trigger_insertable_into = row["is_trigger_insertable_into"].as<string>();

		if (!row["view_text_definition"].is_null()) view.view_text_definition = row["view_text_definition"].as<string>();

		view.schema = view.table_schema; // Schema of object
		view.name = view.table_and_view_name; // Name of object
		return view;
	}

	Index DBProvider::GetIndex(const ObjectData& data) const
	{
		const string query_request =
			ParsingTools::Interpolate("SELECT * FROM pg_indexes"
									  " WHERE tablename not like 'pg%' AND schemaname = '${}'",
									  data.schema) +
			ParsingTools::Interpolate(" AND indexname LIKE '${}';", data.name);

		const auto query_result = Query(query_request);
		const auto row = query_result[0];

		Index index = {};
		index.schema_name = row["schemaname"].as<string>();
		index.table_name = row["tablename"].as<string>();
		index.index_name = row["indexname"].as<string>();

		if (!row["indexdef"].is_null()) index.index_text_definition = row["indexdef"].as<string>();
		return index;
	}

	string DBProvider::GetValue(const string& query_string, const string& column_name) const
	{
		const auto query_result = Query(query_string);
		const auto row = query_result.begin();
		return row[column_name].c_str();
	}

	ScriptDefinition DBProvider::GetTableData(const ObjectData& data, vector<ScriptDefinition>& extra_script_data) const
	{
		Table table = GetTable(data); // Getting information about object
		string script_string;

		// If table is not partition of some other table
		if (!table.IsPartition())
		{
			// "CREATE TABLE" block - initialization of all table's columns
			script_string = "CREATE ";
			if (table.type != "BASE TABLE")
			{
				script_string += table.type = " ";
			}
			script_string += "TABLE " + data.schema + "." + data.name + " (";
			for (const auto& column : table.columns)
			{
				script_string += "\n" + column.name + " " + column.type;
				if (!column.default_value.empty())
				{
					script_string += " DEFAULT " + column.default_value;
				}
				if (!column.IsNullable())
				{
					script_string += " NOT NULL";
				}
				script_string += ",";
			}

			// Creation of constraints
			for (const Constraint& constraint : table.constraints)
			{
				script_string += "\nCONSTRAINT " + constraint.name + " " + constraint.type + " ";
				if (constraint.type == "PRIMARY KEY" || constraint.type == "UNIQUE")
				{
					script_string += "(" + constraint.column_name + ")";
				}
				else if (constraint.type == "FOREIGN KEY")
				{
					script_string += "(" + constraint.column_name + ")\n";
					script_string += "REFERENCES " + constraint.foreign_table_schema + "." + constraint.foreign_table_name +
						" (" + constraint.foreign_column_name + ") ";
					if (constraint.match_option == "NONE")
					{
						script_string += "MATCH SIMPLE";
					}
					else
					{
						script_string += constraint.match_option;
					}
					script_string += "\nON UPDATE " + constraint.on_update;
					script_string += "\nON DELETE " + constraint.on_delete;
				}
				else if (constraint.type == "CHECK")
				{
					script_string += constraint.check_clause;
				}
				script_string += ",";
			}

			if (!table.columns.empty())
			{
				script_string.pop_back(); // Removing an extra comma at the end
			}
			script_string += "\n)\n";

			// "INHERITS" block
			if (!table.inherit_tables.empty())
			{
				script_string += "INHERITS (\n";
				script_string += table.inherit_tables[0];
				for (int inherit_index = 1; inherit_index < table.inherit_tables.size(); ++inherit_index)
				{
					script_string += ",\n" + table.inherit_tables[inherit_index];
				}
				script_string += "\n)\n";
			}

			// "PARTITION BY" block
			if (!table.partition_expression.empty())
			{
				script_string += "PARTITION BY " + table.partition_expression + "\n";
			}

			// "WITH" block to create storage parameters
			script_string += "WITH (\n" + table.options + "\n)\n";

			// "TABLESPACE" definition
			script_string += "TABLESPACE ";
			if (table.space.empty())
			{
				script_string += "pg_default";
			}
			else
			{
				script_string += table.space;
			}
			script_string += ";\n\n";

			// "OWNER TO" block to make the owner user
			script_string += "ALTER TABLE " + data.schema + "." + data.name + " OWNER TO " + table.owner + ";\n\n";

			// "COMMENT ON TABLE" block
			if (!table.description.empty())
			{
				script_string += "COMMENT ON TABLE " + data.schema + "." + data.name + "\nIS '" + table.description +
					"';\n\n";
			}

			// "COMMENT ON COLUMN blocks
			for (const auto& column : table.columns)
			{
				if (!column.description.empty())
				{
					script_string += "COMMENT ON COLUMN " + data.schema + "." + data.name + "." + column.name + "\nIS '" +
						column.description + "';\n\n";
				}
			}

			// Indexes creation
			for (const auto& expression : table.index_create_expressions)
			{
				script_string += expression + ";\n\n";
			}

			// Getting all triggers in table
			string query_request = string(
				"SELECT t.trigger_catalog, t.trigger_schema, t.trigger_name, string_agg(t.event_manipulation, ' or ') ")
				+
				"FROM information_schema.triggers t "
				"WHERE t.event_object_catalog = 'Doors' "
				"AND t.event_object_schema = '" + data.schema + "' "
				"AND t.event_object_table = '" + data.name + "' "
				"GROUP BY t.trigger_catalog, t.trigger_schema, t.trigger_name";

			auto query_result = Query(query_request);
			for (auto row = query_result.begin(); row != query_result.end(); ++row)
			{
				ObjectData triggerObjectData(row["trigger_name"].c_str(), ObjectType::TriggerType, row["trigger_schema"].c_str());
				ScriptDefinition triggerScriptData = GetTriggerData(triggerObjectData);
				extra_script_data.push_back(triggerScriptData);
			}
		}
		else
		{
			PartitionTable parent_table = table.GetPartitionTable();
			script_string = "CREATE TABLE " + data.schema + "." + data.name +
				" PARTITION OF " + parent_table.schema + "." + parent_table.name + "\n" +
				parent_table.partition_expression + ";\n";
		}

		auto script_data = ScriptDefinition(data, script_string);
		script_data.name += ".sql";
		return script_data;
	}

	ScriptDefinition DBProvider::GetFunctionData(const ObjectData& data) const
	{
		const Function function = GetFunction(data); // Getting information about object
		ScriptDefinition scriptData = ScriptDefinition(data, function.function_text_definition);
		scriptData.name += ".sql";
		return scriptData;
	}

	ScriptDefinition DBProvider::GetViewData(const ObjectData& data) const
	{
		const View view = GetView(data); // Getting information about object
		ScriptDefinition scriptData = ScriptDefinition(data, view.view_text_definition);
		scriptData.name += ".sql";
		return scriptData;
	}

	ScriptDefinition DBProvider::GetSequenceData(const ObjectData& data) const
	{
		const Sequence sequence = GetSequence(data); // Getting information about object
		ScriptDefinition scriptData = ScriptDefinition(data, sequence.query_create_seq);
		scriptData.name += ".sql";
		return scriptData;
	}

	ScriptDefinition DBProvider::GetTriggerData(const ObjectData& data) const
	{
		const Trigger trigger = GetTrigger(data); // Getting information about object
		ScriptDefinition scriptData = ScriptDefinition(data, trigger.trigger_text_definition);
		scriptData.name += ".sql";
		return scriptData;
	}

	ScriptDefinition DBProvider::GetIndexData(const ObjectData& data) const
	{
		const Index index = GetIndex(data); // Getting information about object
		ScriptDefinition scriptData = ScriptDefinition(data, index.index_text_definition);
		scriptData.name += ".sql";
		return scriptData;
	}

	bool DBProvider::InitializePartitionTable(Table& table, const ObjectData& data) const
	{
		const string query_request = ""
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

		const auto query_result = Query(query_request);
		if (query_result.empty())
		{
			return false;
		}

		const auto row = query_result.begin();
		const string parent_schema = row["parent_schema"].c_str();
		const string parent_name = row["parent_name"].c_str();
		const string partition_expression = row["partition_expression"].c_str();
		table.SetPartitionTable(parent_schema, parent_name, partition_expression);

		return true;
	}

	void DBProvider::InitializeType(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT * FROM information_schema.tables t "
			"WHERE t.table_schema = '" + data.schema +
			"' AND t.table_name = '" + data.name + "'";
		table.type = GetValue(query_request, "table_type");
	}

	void DBProvider::InitializeOwner(Table& table, const ObjectData& data) const
	{
		const string query_request =
			"SELECT * FROM pg_tables t where schemaname = '" + data.schema +
			"' and tablename = '" + data.name + "'";
		table.owner = GetValue(query_request, "tableowner");
	}

	void DBProvider::InitializeDescription(Table& table, const ObjectData& data) const
	{
		const string query_request =
			"SELECT obj_description('" + data.schema + "." + data.name + "'::regclass::oid)";
		table.description = GetValue(query_request, "obj_description");
	}

	void DBProvider::InitializeOptions(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT * FROM pg_class WHERE relname = '" + data.name + "'";
		auto query_value = GetValue(query_request, "reloptions");

		// Getting OIDS value
		string oids_expression = "OIDS=false";
		if (GetValue(query_request, "relhasoids") == "t")
		{
			oids_expression = "OIDS=true";
		}

		table.options += oids_expression;

		if (!query_value.empty())
		{
			query_value.erase(0, 1); // Remove { symbol from beginning
			query_value.pop_back(); // Remove } symbol from ending

			vector<string> expression_string = ParsingTools::SplitToVector(query_value, ",");
			for (const auto& expression_index : expression_string)
			{
				table.options += ",\n" + expression_index;
			}
		}
	}

	void DBProvider::InitializeSpace(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT * FROM pg_tables WHERE tablename = '" + data.name + "' AND schemaname = '" + data.schema + "'";
		table.space = GetValue(query_request, "tablespace");
	}

	void DBProvider::InitializeColumns(Table& table, const ObjectData& data) const
	{
		const string query_request = "select c.table_catalog, "
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
			"and c.table_catalog = '" + current_connection_->GetParameters().database + "' "
			"and c.table_schema = '" + data.schema + "' "
			"and c.table_name = '" + data.name + "'";

		const auto query_result = Query(query_request); // SQL Query result, contains information in table format
		for (auto row = query_result.begin(); row != query_result.end(); ++row)
		{
			Column column;
			column.name = row["column_name"].c_str();
			column.type = row["collumn_type"].c_str();
			column.default_value = row["column_default"].c_str();
			column.description = row["description"].c_str();
			column.SetNullable(row["is_nullable"].c_str());

			table.columns.push_back(column);
		}
	}

	void DBProvider::InitializePartitionExpression(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT c.relnamespace::regnamespace::text, c.relname, pg_get_partkeydef(c.oid) AS partition_expression "
			"FROM   pg_class c "
			"WHERE  1 = 1 "
			"AND c.relkind = 'p' "
			"AND c.relname = '" + data.name + "' "
			"AND  c.relnamespace::regnamespace::text = '" + data.schema + "'";

		const auto query_result = Query(query_request);

		if (!query_result.empty())
		{
			const auto row = query_result.begin();
			table.partition_expression = row["partition_expression"].c_str();
		}

	}

	void DBProvider::InitializeConstraints(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT "
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

		const auto query_result = Query(query_request);

		for (auto row = query_result.begin(); row != query_result.end(); ++row)
		{
			Constraint constraint;
			constraint.type = row["constraint_type"].c_str();
			constraint.name = row["constraint_name"].c_str();
			constraint.column_name = row["column_name"].c_str();
			constraint.check_clause = row["check_clause"].c_str();
			constraint.foreign_table_schema = row["foreign_table_schema"].c_str();
			constraint.foreign_table_name = row["foreign_table_name"].c_str();
			constraint.foreign_column_name = row["foreign_column_name"].c_str();
			constraint.match_option = row["match_option"].c_str();
			constraint.on_delete = row["delete_rule"].c_str();
			constraint.on_update = row["update_rule"].c_str();

			table.constraints.push_back(constraint);
		}
	}

	void DBProvider::InitializeInheritTables(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT "
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

		const auto result = Query(query_request);
		for (auto row = result.begin(); row != result.end(); ++row)
		{
			table.inherit_tables.emplace_back(row["parent_name"].c_str());
		}
	}

	void DBProvider::InitializeIndexExpressions(Table& table, const ObjectData& data) const
	{
		const string query_request = "SELECT * FROM pg_indexes "
			"WHERE 1 = 1 "
			"AND schemaname = '" + data.schema + "' "
			"AND tablename = '" + data.name + "'";

		const auto query_result = Query(query_request);
		for (auto row = query_result.begin(); row != query_result.end(); ++row)
		{
			table.index_create_expressions.emplace_back(row["indexdef"].c_str());
		}
	}

	void PrintObjectsData(const pqxx::result& query_result)
	{
		// Iterate over the rows in our result set.
		// Result objects are containers similar to vector and such.
		for (auto row = query_result.begin();
			 row != query_result.end();
			 ++row)
		{
			cout << row["obj_schema"].as<string>() << "\t"
				<< row["obj_name"].as<string>() << "\t"
				<< row["obj_type"].as<string>() << endl;
		}
	}
}
