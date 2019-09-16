#include "DBProvider/DBProvider.h"
#include "Shared/ParsingTools.h"
#include <iostream>
#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <string>
#include <utility>

using namespace std;

namespace Provider {

ObjectData::ObjectData(const string &p_name, const ObjectType &p_type,
                       const string &p_scheme,
                       const vector<string> &p_params_vector) {
  name = p_name;
  type = p_type;
  schema = p_scheme;
  params = p_params_vector;
}

bool ObjectData::operator==(ObjectData &object) const {
  return (this->name == object.name) && (this->type == object.type);
}

ScriptDefinition::ScriptDefinition(const string &p_name,
                                   const ObjectType &p_type,
                                   const string &p_scheme,
                                   const vector<string> &p_params_vector,
                                   string p_text)
    : ObjectData(p_name, p_type, p_scheme, p_params_vector) {
  text = move(p_text);
}

ScriptDefinition::ScriptDefinition(const ObjectData &object_data,
                                   const string &p_text)
    : ScriptDefinition(object_data.name, object_data.type, object_data.schema,
                       object_data.params, p_text) {
  text = p_text;
}

bool Column::IsNullable() const { return this->nullable_; }

void Column::SetNullable(string value) {
  transform(value.begin(), value.end(), value.begin(), tolower);

  if (value == "yes") {
    this->nullable_ = true;
    return;
  }

  this->nullable_ = false;
}

void TableStructure::SetPartitionTable(string schema, string name,
                                       string partition_expression) {
  this->partition_table_.name = move(name);
  this->partition_table_.schema = move(schema);
  this->partition_table_.partition_expression = move(partition_expression);
  this->i_partition_ = true;
}

auto TableStructure::GetPartitionTable() const -> PartitionTable {
  return partition_table_;
}

bool TableStructure::IsPartition() const { return i_partition_; }

string CastObjectType(const ObjectType &object_type) {
  switch (object_type) {
  case ObjectType::Empty:
    return string();
  case ObjectType::Table:
    return "table";
  case ObjectType::Function:
    return "function";
  case ObjectType::Trigger:
    return "trigger";
  case ObjectType::View:
    return "view";
  case ObjectType::Index:
    return "index";
  case ObjectType::Sequence:
    return "sequence";
  }
  throw invalid_argument("ERROR: No such object type.\n");
}

ObjectType CastObjectType(const string &object_type) {
  if (object_type == "table")
    return ObjectType::Table;
  if (object_type == "function")
    return ObjectType::Function;
  if (object_type == "trigger")
    return ObjectType::Trigger;
  if (object_type == "view")
    return ObjectType::View;
  if (object_type == "index")
    return ObjectType::Index;
  if (object_type == "sequence")
    return ObjectType::Sequence;

  throw invalid_argument("ERROR: No such object type.\n");
}

DBProvider::DBProvider() { current_connection_ = make_shared<DBConnection>(); };

DBProvider::DBProvider(const string &connection_params) {
  current_connection_ = make_shared<DBConnection>();
  current_connection_->SetConnection(const_cast<string &>(connection_params));
}

DBProvider::DBProvider(shared_ptr<DBConnection> already_set_connection) {
  if (!already_set_connection->IsParametersSet()) {
    throw invalid_argument(
        "[DBProvider]: Passed connection was not set properly.\n");
  }

  current_connection_ = move(already_set_connection);
}

DBProvider::~DBProvider() {
  if (current_connection_ != nullptr) {
    current_connection_.reset();
  }
}

void DBProvider::SetConnection(std::string &connection_parameters) const {
  current_connection_->SetConnection(connection_parameters);
}

void DBProvider::Connect() {
  current_connection_->Connect();
  InitializeStatements();
  PrepareAllStatements();
}

void DBProvider::InitializeStatements() {
  const auto database = current_connection_->GetParameters().database;

  prepared_statements_["get_all_triggers in table"] =
      "SELECT t.trigger_catalog, t.trigger_schema, t.trigger_name, "
      "string_agg(t.event_manipulation, ' or ')"
      "FROM information_schema.triggers t WHERE "
      "t.event_object_catalog = '" +
      database +
      "' AND t.event_object_schema = $1 AND t.event_object_table = $2 "
      "GROUP BY t.trigger_catalog, t.trigger_schema, t.trigger_name ";

  prepared_statements_["table_exists"] =
      "SELECT EXISTS (SELECT * FROM information_schema.tables WHERE "
      "table_schema = $1 AND table_name = $2);";

  prepared_statements_["function_exists"] =
      "SELECT EXISTS (SELECT * FROM information_schema.routines r, "
      "r.specific_schema = $1 "
      "AND r.routine_name||'('||COALESCE(array_to_string(p.proargnames, ',', "
      "'*'),'')||')' = $2 "
      "AND r.external_language = 'PLPGSQL' "
      "AND r.routine_name = p.proname "
      "AND r.specific_name = p.proname || '_' || p.oid);";

  prepared_statements_["trigger_exists"] = "SELECT EXISTS (SELECT * "
                                           "FROM information_schema.triggers "
                                           "WHERE trigger_schema = $1 "
                                           "AND trigger_name = $2);";

  prepared_statements_["view_exists"] = "SELECT EXISTS (SELECT * "
                                        "FROM information_schema.views "
                                        "WHERE table_schema = $1 "
                                        "AND table_name = $2);";

  prepared_statements_["sequence_exists"] = "SELECT EXISTS (SELECT * "
                                            "FROM information_schema.sequences "
                                            "WHERE sequence_schema = $1 "
                                            "AND sequence_name = $2);";

  prepared_statements_["index_exists"] =
      "SELECT EXISTS (SELECT * FROM pg_indexes "
      "WHERE schemaname = $1 AND indexname = $2);";

  prepared_statements_["get_function_text"] =
      "SELECT * , array_to_string(p.proargnames, ', ', '*'), "
      "pg_get_functiondef(p.oid) "
      "FROM information_schema.routines r, pg_catalog.pg_proc p "
      "WHERE r.external_language = 'PLPGSQL' "
      "AND r.routine_name = p.proname "
      "AND r.routine_schema = $1 "
      "AND r.specific_name = $2 || '_' || p.oid "
      "AND array_to_string(p.proargnames, ', ', '*') = $3";

  prepared_statements_["get_trigger_text"] =
      "SELECT t.trigger_catalog, t.trigger_schema, t.trigger_name, "
      "string_agg(t.event_manipulation, ' or '), pg_get_triggerdef(t.oid)"
      "FROM information_schema.triggers t "
      "WHERE t.event_object_catalog = '" +
      database +
      "' AND t.event_object_schema = $1"
      "AND t.event_object_table = $2 "
      "GROUP BY t.trigger_catalog, t.trigger_schema, t.trigger_name;";

  prepared_statements_["get_view_text"] =
      "SELECT * FROM information_schema.views WHERE "
      "table_catalog = '" +
      database + "' AND table_name = $1;";

  prepared_statements_["get_index_text"] =
      "SELECT * FROM pg_indexes WHERE tablename NOT LIKE 'pg%' "
      "AND schemaname = $1 AND indexname LIKE $2;";

  prepared_statements_["get_sequence_text"] =
      "SELECT * FROM information_schema.sequences WHERE "
      "sequence_catalog = '" +
      database + "' AND sequence_name = $1 AND sequence_schema = $2;";
}

void DBProvider::PrepareAllStatements() {
  InitializeStatements();
  auto connection = current_connection_->GetConnection();
  for (auto &statement : prepared_statements_) {
    connection->prepare(statement.first, statement.second);
  }
}

void DBProvider::PrepareStatement(const string &key,
                                  const string &statement_definition) const {
  auto connection = current_connection_->GetConnection();
  connection->prepare(key, statement_definition);
}

void DBProvider::PrepareStatements(
    const map<string, string> &statements) const {
  auto connection = current_connection_->GetConnection();
  for (auto &x : statements) {
    connection->prepare(x.first, x.second);
  }
}

void DBProvider::ExecutePreparedWithoutArgs(
    const vector<string> &statement_keys) const {
  const auto connection = current_connection_->GetConnection();
  pqxx::work transaction(*connection, "execute prepared without args");

  vector<pqxx::result> results;
  results.reserve(statement_keys.size());

  for (const auto &key : statement_keys) {
    results.emplace_back(transaction.exec_prepared(key));
  }
}

vector<ObjectData> DBProvider::GetObjects() const {
  // example:
  // output - public, myFunction, function, <param1, param2, param3>
  //          common, myTable,    table,    <>
  const string get_objects = "SELECT /*sequences */"
                             "f.sequence_schema AS obj_schema,"
                             "f.sequence_name AS obj_name,"
                             "'Sequence' AS obj_type "
                             "FROM information_schema.sequences f "
                             "UNION ALL "
                             "SELECT /*tables */ "
                             "f.table_schema AS obj_schema,"
                             "f.table_name AS obj_name,"
                             "'table' AS obj_type "
                             "FROM information_schema.tables f "
                             "WHERE f.table_schema in"
                             "('public', 'io', 'common', 'secure');";

  const auto all_objects = Query(get_objects);
  ObjectsDataVector objects;

  for (auto row = all_objects.begin(); row != all_objects.end(); ++row) {
    const vector<string> parameters;
    objects.emplace_back(
        ObjectData(row["obj_name"].as<string>(),
                   CastObjectType(row["obj_type"].as<string>()),
                   row["obj_schema"].as<string>(), parameters));
  }

  return objects;
}

ScriptDefinition
DBProvider::GetScriptData(const ObjectData &data,
                          vector<ScriptDefinition> &extra_script_data) const {
  ScriptDefinition script(data, "");
  switch (data.type) {
  case ObjectType::Table:
    script = GetTableData(data, extra_script_data);
  case ObjectType::Function:
    script = FunctionDefinition(data);
  case ObjectType::Trigger:
    script = TriggerDefinition(data);
  case ObjectType::View:
    script = ViewDefinition(data);
  case ObjectType::Index:
    script = IndexDefinition(data);
  case ObjectType::Sequence:
    script = SequenceDefinition(data);
  }

  // Add .sql format and return result
  script.name += ".sql";
  return script;
}

// Checks if specified object exists in database
bool DBProvider::DoesCurrentObjectExists(const string &scheme,
                                         const string &signature,
                                         const string &type) const {
  switch (CastObjectType(type)) {
  case ObjectType::Table:
    return TableExists(scheme, signature);
  case ObjectType::Function:
    return FunctionExists(scheme, signature);
  case ObjectType::Trigger:
    return TriggerExists(scheme, signature);
  case ObjectType::View:
    return ViewExists(scheme, signature);
  case ObjectType::Index:
    return IndexExists(scheme, signature);
  case ObjectType::Sequence:
    return SequenceExists(scheme, signature);
  default:
    return false;
  }
}

pqxx::result DBProvider::Query(const string &sql_request) const {
  if (!current_connection_->IsOpen()) {
    throw runtime_error("ERROR: Couldn't execute query. Database connection "
                        "was lost or closed.\n");
  }

  pqxx::work transaction(*current_connection_->GetConnection(),
                         "query transaction");

  // Get result from database
  try {
    auto query_result = transaction.exec(sql_request);
    transaction.commit();
    return query_result;
  } catch (const pqxx::sql_error &err) {
    cerr << "Failed to execute query" << endl;
    cerr << "SQL error: " << err.what() << endl;
    cerr << "Query was: " << err.query() << endl;
    throw runtime_error("ERROR: Couldn't execute query. SQL error occured.\n" +
                        string(err.what()) + "\n");
  }
}

pair<bool, pqxx::result>
DBProvider::QueryWithStatus(const string &sql_request) const {
  if (!current_connection_->IsOpen()) {
    throw runtime_error(
        "ERROR: Couldn't execute query. Database connection is dead.\n");
  }
  pqxx::work transaction(*current_connection_->GetConnection(),
                         "query transaction");
  pqxx::result query_result;
  // Get results from database
  try {
    query_result = transaction.exec(sql_request);
    transaction.commit();
    return make_pair(true, query_result);
  } catch (const pqxx::sql_error &err) {
    cerr << err.what() << endl;
    return make_pair(false, query_result);
  }
}

bool DBProvider::TableExists(const string &table_schema,
                             const string &table_name) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  const auto query_result =
      transaction.exec_prepared("table_exists", table_schema, table_name);
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

bool DBProvider::SequenceExists(const string &sequence_schema,
                                const string &sequence_name) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  const auto query_result = transaction.exec_prepared(
      "sequence_exists", sequence_schema, sequence_name);
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

bool DBProvider::IndexExists(const string &index_schema,
                             const string &index_name) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  const auto query_result =
      transaction.exec_prepared("index_exists", index_schema, index_name);
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

bool DBProvider::FunctionExists(const string &function_schema,
                                const string &function_signature) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  const auto query_result = transaction.exec_prepared(
      "function_exists", function_schema, function_signature);
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

bool DBProvider::ViewExists(const string &view_schema,
                            const string &view_name) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  const auto query_result =
      transaction.exec_prepared("view_exists", view_schema, view_name);
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

bool DBProvider::TriggerExists(const string &trigger_schema,
                               const string &trigger_name) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  const auto query_result =
      transaction.exec_prepared("trigger_exists", trigger_schema, trigger_name);
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

std::map<std::string, std::string> DBProvider::GetPreparedStatements() const {
  return prepared_statements_;
}

TableStructure DBProvider::GetTable(const ObjectData &data) const {
  TableStructure table;
  // If table is partition of some other table
  // need to know only parent
  if (!InitializePartitionTable(table, data)) {
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

ScriptDefinition DBProvider::FunctionDefinition(const ObjectData &data) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  auto res =
      transaction.exec_prepared("get_function_text", data.schema, data.name,
                                ParsingTools::JoinAsString(data.params, ", "));

  const auto row = res[0];
  auto text_definition = string();
  if (!row["pg_get_functiondef"].is_null()) {
    text_definition = row["pg_get_functiondef"].as<string>();
  }

  ScriptDefinition function;
  function.schema = row["routine_schema"].as<string>(); // Function schema
  function.name = row["specific_name"].as<string>() + "(" +
                  row["array_to_string"].as<string>() + ")"; // Function name
  function.params = data.params;
  function.text = text_definition;
  return function;
}

ScriptDefinition DBProvider::TriggerDefinition(const ObjectData &data,
                                               const string &comment,
                                               const string &code) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  pqxx::result res =
      transaction.exec_prepared("get_trigger_text", data.schema, data.name);

  const auto row = res[0];

  ScriptDefinition trigger;
  trigger.schema = row["trigger_schema"].as<string>(); // Trigger schema
  trigger.name = row["trigger_name"].as<string>();     // Trigger name

  string text_definition;
  if (row["pg_get_triggerdef"].is_null()) {
    text_definition = "CREATE FUNCTION " + trigger.schema + ".'" +
                      trigger.name +
                      "'()\n	RETURNS "
                      "trigger\n	LANGUAGE 'plpgsql'\n"
                      "	NOT LEAKPROOF \n	AS $BODY$" +
                      code +
                      "$BODY$;\n\n"
                      "ALTER FUNCTION " +
                      trigger.schema + ".'" + trigger.name +
                      "'()\n"
                      "	OWNER TO " +
                      current_connection_->GetParameters().username +
                      ";\n\n"
                      "COMMENT ON FUNCTION " +
                      trigger.schema + ".'" + trigger.name + "'()\n" +
                      "	IS '" + comment + "';\n";

  } else {
    text_definition = row["pg_get_triggerdef"].as<string>();
  }

  trigger.text = text_definition;
  return trigger;
}

ScriptDefinition
DBProvider::SequenceDefinition(const ObjectData &data, int start_value,
                               int minimum_value, int maximum_value,
                               int increment, bool cycle_option,
                               const string &comment) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  pqxx::result res =
      transaction.exec_prepared("get_sequence_text", data.name, data.schema);
  const auto row = res[0];

  ScriptDefinition sequence;
  sequence.schema = row["sequence_schema"].as<string>(); // Sequence schema
  sequence.name = row["sequence_name"].as<string>();     // Sequence name
  sequence.text =
      "CREATE SEQUENCE " + sequence.schema + ".'" + sequence.name + "'\n";

  if (cycle_option) {
    sequence.text += "	CYCLE\n";
  }

  sequence.text += "	INCREMENT " + to_string(increment) +
                   "\n"
                   "	START " +
                   to_string(start_value) +
                   "\n"
                   "	MINVALUE " +
                   to_string(minimum_value) +
                   "\n"
                   " MAXVALUE " +
                   to_string(maximum_value) + ";\n\n ALTER SEQUENCE " +
                   sequence.schema + ".'" + sequence.name +
                   "'\n	OWNER TO " +
                   current_connection_->GetParameters().username +
                   ";\n\nCOMMENT ON SEQUENCE " + sequence.schema + ".'" +
                   sequence.name + "'\n	IS '" + comment + "';";
  return sequence;
}

ScriptDefinition DBProvider::ViewDefinition(const ObjectData &data) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  pqxx::result res =
      transaction.exec_prepared("get_view_text", data.name, data.schema);
  const auto row = res[0];

  ScriptDefinition view;
  view.schema = row["table_schema"].as<string>();       // View schema
  view.name = row["table_and_view_name"].as<string>();  // View name
  view.text = row["view_text_definition"].as<string>(); // View text
  return view;
}

ScriptDefinition DBProvider::IndexDefinition(const ObjectData &data) const {
  pqxx::work transaction(*current_connection_->GetConnection());
  pqxx::result res =
      transaction.exec_prepared("get_index_text", data.schema, data.name);
  const auto row = res[0];
  ScriptDefinition index;

  index.schema = row["schemaname"].as<string>();
  index.name = row["indexname"].as<string>();

  if (!row["indexdef"].is_null()) {
    index.text = row["indexdef"].as<string>();
  }

  return index;
}

string DBProvider::GetValue(const string &sql_request,
                            const string &column_name) const {
  const auto query_result = Query(sql_request);
  const auto row = query_result.begin();
  return row[column_name].c_str();
}

ScriptDefinition
DBProvider::GetTableData(const ObjectData &data,
                         vector<ScriptDefinition> &extra_script_data) const {

  auto table = GetTable(data); // Getting information about object
  string script_text;

  // If table is not partition of some other table
  if (!table.IsPartition()) {
    // "CREATE TABLE" block - initialization of all table's columns
    script_text = "CREATE ";
    if (table.type != "BASE TABLE") {
      script_text += table.type = " ";
    }
    script_text += "TABLE " + data.schema + "." + data.name + " (";
    for (const auto &column : table.columns) {
      script_text += "\n" + column.name + " " + column.type;
      if (!column.default_value.empty()) {
        script_text += " DEFAULT " + column.default_value;
      }
      if (!column.IsNullable()) {
        script_text += " NOT NULL";
      }
      script_text += ",";
    }

    // Creation of constraints
    for (const Constraint &constraint : table.constraints) {
      script_text +=
          "\nCONSTRAINT " + constraint.name + " " + constraint.type + " ";
      if (constraint.type == "PRIMARY KEY" || constraint.type == "UNIQUE") {
        script_text += "(" + constraint.column_name + ")";
      } else if (constraint.type == "FOREIGN KEY") {
        script_text += "(" + constraint.column_name + ")\n";
        script_text += "REFERENCES " + constraint.foreign_table_schema + "." +
                       constraint.foreign_table_name + " (" +
                       constraint.foreign_column_name + ") ";
        if (constraint.match_option == "NONE") {
          script_text += "MATCH SIMPLE";
        } else {
          script_text += constraint.match_option;
        }
        script_text += "\nON UPDATE " + constraint.on_update;
        script_text += "\nON DELETE " + constraint.on_delete;
      } else if (constraint.type == "CHECK") {
        script_text += constraint.check_clause;
      }
      script_text += ",";
    }

    if (!table.columns.empty()) {
      script_text.pop_back(); // Removing an extra comma at the end
    }
    script_text += "\n)\n";

    // "INHERITS" block
    if (!table.inherit_tables.empty()) {
      script_text += "INHERITS (\n";
      script_text += table.inherit_tables[0];
      for (auto inherit_index = 1; inherit_index < table.inherit_tables.size();
           ++inherit_index) {
        script_text += ",\n" + table.inherit_tables[inherit_index];
      }
      script_text += "\n)\n";
    }

    // "PARTITION BY" block
    if (!table.partition_expression.empty()) {
      script_text += "PARTITION BY " + table.partition_expression + "\n";
    }

    // "WITH" block to create storage parameters
    script_text += "WITH (\n" + table.options + "\n)\n";

    // "TABLESPACE" definition
    script_text += "TABLESPACE ";
    if (table.space.empty()) {
      script_text += "pg_default";
    } else {
      script_text += table.space;
    }
    script_text += ";\n\n";

    // "OWNER TO" block to make the owner user
    script_text += "ALTER TABLE " + data.schema + "." + data.name +
                   " OWNER TO " + table.owner + ";\n\n";

    // "COMMENT ON TABLE" block
    if (!table.description.empty()) {
      script_text += "COMMENT ON TABLE " + data.schema + "." + data.name +
                     "\nIS '" + table.description + "';\n\n";
    }

    // "COMMENT ON COLUMN blocks
    for (const auto &column : table.columns) {
      if (!column.description.empty()) {
        script_text += "COMMENT ON COLUMN " + data.schema + "." + data.name +
                       "." + column.name + "\nIS '" + column.description +
                       "';\n\n";
      }
    }

    // Indexes creation
    for (const auto &expression : table.index_create_expressions) {
      script_text += expression + ";\n\n";
    }

    // Use prepared statement to get all triggers from current table
    pqxx::work transaction(*current_connection_->GetConnection());
    auto query_result = transaction.exec_prepared("get_all_triggers in table",
                                                  data.schema, data.name);

    for (auto row = query_result.begin(); row != query_result.end(); ++row) {
      ObjectData trigger_object_data(row["trigger_name"].c_str(),
                                     ObjectType::Trigger,
                                     row["trigger_schema"].c_str());
      ScriptDefinition trigger_script_data =
          TriggerDefinition(trigger_object_data);
      extra_script_data.push_back(trigger_script_data);
    }
  } else {
    PartitionTable parent_table = table.GetPartitionTable();
    script_text = "CREATE TABLE " + data.schema + "." + data.name +
                  " PARTITION OF " + parent_table.schema + "." +
                  parent_table.name + "\n" + parent_table.partition_expression +
                  ";\n";
  }

  auto script_definition = ScriptDefinition(data, script_text);
  script_definition.name += ".sql";
  return script_definition;
}

bool DBProvider::InitializePartitionTable(TableStructure &table,
                                          const ObjectData &data) const {
  const string query_request =
      ""
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
      "AND c.relname = '" +
      data.name +
      "' "
      "AND n.nspname = '" +
      data.schema +
      "' "
      "AND c.relispartition = 'true'";

  const auto query_result = Query(query_request);
  if (query_result.empty()) {
    return false;
  }

  const auto row = query_result.begin();
  const string parent_schema = row["parent_schema"].c_str();
  const string parent_name = row["parent_name"].c_str();
  const string partition_expression = row["partition_expression"].c_str();
  table.SetPartitionTable(parent_schema, parent_name, partition_expression);

  return true;
}

void DBProvider::InitializeType(TableStructure &table,
                                const ObjectData &data) const {
  const auto query_request = "SELECT * FROM information_schema.tables t "
                             "WHERE t.table_schema = '" +
                             data.schema + "' AND t.table_name = '" +
                             data.name + "'";
  table.type = GetValue(query_request, "table_type");
}

void DBProvider::InitializeOwner(TableStructure &table,
                                 const ObjectData &data) const {
  const auto query_request = "SELECT * FROM pg_tables t where schemaname = '" +
                             data.schema + "' and tablename = '" + data.name +
                             "'";
  table.owner = GetValue(query_request, "tableowner");
}

void DBProvider::InitializeDescription(TableStructure &table,
                                       const ObjectData &data) const {
  const auto query_request = "SELECT obj_description('" + data.schema + "." +
                             data.name + "'::regclass::oid)";
  table.description = GetValue(query_request, "obj_description");
}

void DBProvider::InitializeOptions(TableStructure &table,
                                   const ObjectData &data) const {
  const auto query_request =
      "SELECT * FROM pg_class WHERE relname = '" + data.name + "'";
  auto query_value = GetValue(query_request, "reloptions");

  // Getting OIDS value
  string oids_expression = "OIDS=false";
  if (GetValue(query_request, "relhasoids") == "t") {
    oids_expression = "OIDS=true";
  }

  table.options += oids_expression;

  if (!query_value.empty()) {
    query_value.erase(0, 1); // Remove { symbol from beginning
    query_value.pop_back();  // Remove } symbol from ending

    vector<string> expression_string =
        ParsingTools::SplitToVector(query_value, ",");
    for (const auto &expression_index : expression_string) {
      table.options += ",\n" + expression_index;
    }
  }
}

void DBProvider::InitializeSpace(TableStructure &table,
                                 const ObjectData &data) const {
  const auto query_request = "SELECT * FROM pg_tables WHERE tablename = '" +
                             data.name + "' AND schemaname = '" + data.schema +
                             "'";
  table.space = GetValue(query_request, "tablespace");
}

void DBProvider::InitializeColumns(TableStructure &table,
                                   const ObjectData &data) const {
  const auto query_request =
      "select c.table_catalog, "
      "c.table_schema, "
      "c.table_name, "
      "c.column_name, "
      "c.is_nullable, "
      "format_type(pa.atttypid, pa.atttypmod) as collumn_type, "
      "c.column_default, "
      "pd.description "
      "from information_schema.columns c "
      "join pg_class pc on(pc.oid = (c.table_schema || '.' || c.table_name) "
      "::regclass::oid) "
      "join pg_attribute pa on(pa.attrelid = pc.oid and pa.attname = "
      "c.column_name) "
      "left join pg_catalog.pg_description pd on(pd.objoid = (c.table_schema "
      "|| '.' || c.table_name)::regclass::oid and pd.objsubid = "
      "c.ordinal_position) "
      "where 1 = 1 "
      "and c.table_catalog = '" +
      current_connection_->GetParameters().database +
      "' "
      "and c.table_schema = '" +
      data.schema +
      "' "
      "and c.table_name = '" +
      data.name + "'";

  const auto query_result = Query(
      query_request); // SQL Query result, contains information in table format
  for (auto row = query_result.begin(); row != query_result.end(); ++row) {
    Column column;
    column.name = row["column_name"].c_str();
    column.type = row["collumn_type"].c_str();
    column.default_value = row["column_default"].c_str();
    column.description = row["description"].c_str();
    column.SetNullable(row["is_nullable"].c_str());

    table.columns.push_back(column);
  }
}

void DBProvider::InitializePartitionExpression(TableStructure &table,
                                               const ObjectData &data) const {
  const auto query_request =
      "SELECT c.relnamespace::regnamespace::text, c.relname, "
      "pg_get_partkeydef(c.oid) AS partition_expression "
      "FROM   pg_class c "
      "WHERE  1 = 1 "
      "AND c.relkind = 'p' "
      "AND c.relname = '" +
      data.name +
      "' "
      "AND  c.relnamespace::regnamespace::text = '" +
      data.schema + "'";

  const auto query_result = Query(query_request);

  if (!query_result.empty()) {
    const auto row = query_result.begin();
    table.partition_expression = row["partition_expression"].c_str();
  }
}

void DBProvider::InitializeConstraints(TableStructure &table,
                                       const ObjectData &data) const {
  const auto query_request =
      "SELECT "
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
      "WHERE tc.table_name = '" +
      data.name +
      "' "
      "AND tc.table_schema = '" +
      data.schema +
      "' "
      "AND COALESCE(cc.check_clause, '') NOT ILIKE '%IS NOT NULL%' ";

  const auto query_result = Query(query_request);

  for (auto row = query_result.begin(); row != query_result.end(); ++row) {
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

void DBProvider::InitializeInheritTables(TableStructure &table,
                                         const ObjectData &data) const {
  const auto query_request =
      "SELECT "
      "nmsp_parent.nspname AS parent_schema, "
      "parent.relname      AS parent_name, "
      "nmsp_child.nspname  AS child_schema, "
      "child.relname       AS child_name "
      "FROM pg_inherits "
      "JOIN pg_class parent            ON pg_inherits.inhparent = parent.oid "
      "JOIN pg_class child             ON pg_inherits.inhrelid = child.oid "
      "JOIN pg_namespace nmsp_parent   ON nmsp_parent.oid = "
      "parent.relnamespace "
      "JOIN pg_namespace nmsp_child    ON nmsp_child.oid = child.relnamespace "
      "WHERE child.relispartition = 'false' "
      "AND child.relname = '" +
      data.name +
      "' "
      "AND nmsp_child.nspname = '" +
      data.schema + "'";

  const auto result = Query(query_request);
  for (auto row = result.begin(); row != result.end(); ++row) {
    table.inherit_tables.emplace_back(row["parent_name"].c_str());
  }
}

void DBProvider::InitializeIndexExpressions(TableStructure &table,
                                            const ObjectData &data) const {
  const auto query_request = "SELECT * FROM pg_indexes "
                             "WHERE 1 = 1 "
                             "AND schemaname = '" +
                             data.schema +
                             "' "
                             "AND tablename = '" +
                             data.name + "'";

  const auto query_result = Query(query_request);
  for (auto row = query_result.begin(); row != query_result.end(); ++row) {
    table.index_create_expressions.emplace_back(row["indexdef"].c_str());
  }
}

void PrintObjectsData(const pqxx::result &query_result) {
  // Iterate over the rows in our result set.
  // Result objects are containers similar to vector and such.
  for (auto row = query_result.begin(); row != query_result.end(); ++row) {
    cout << row["obj_schema"].as<string>() << "\t"
         << row["obj_name"].as<string>() << "\t" << row["obj_type"].as<string>()
         << endl;
  }
}
} // namespace Provider
