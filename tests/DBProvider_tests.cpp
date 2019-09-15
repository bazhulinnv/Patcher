#include <gtest/gtest.h>

#include "Shared/Logger.h"
#include "Shared/ParsingTools.h"
#include "Shared/TextTable.h"

#include "DBProvider/DBProvider.h"

#include <functional>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <thread>

using namespace std;
using namespace Provider;

bool Test_prepared()
{
  auto conn_str = LoginData("127.0.0.1:5432:Doors:doo:rs").LoginString_Pqxx();
  auto conn = pqxx::connection(conn_str);
  pqxx::work transaction(conn);
  auto sequence_exists =	"SELECT EXISTS (SELECT * "
							"FROM information_schema.sequences "
							"WHERE sequence_schema = $1 "
							"AND sequence_name = $2);";
  conn.prepare("sequence_exists", sequence_exists);
  const auto query_result = transaction.exec_prepared("sequence_exists", "public", "math_blob_id_seq");
  transaction.commit();
  return query_result.begin()["exists"].as<bool>();
}

bool Test_sequenceExists(shared_ptr<DBProvider> db_prov) {
  return db_prov->SequenceExists("public", "math_blob_id_seq");
}

bool Test_tableExists(shared_ptr<DBProvider> db_prov) {
  return db_prov->TableExists("public", "errors");
}

bool Test_PrintTableList(shared_ptr<DBProvider> db_prov) {
  pqxx::result query_result;

  const string request_sql = "SELECT * FROM pg_catalog.pg_tables "
                             "WHERE schemaname != 'pg_catalog' "
                             "AND schemaname != 'information_schema'; ";
  try {
    // Get all tables from database
    query_result = db_prov->Query(request_sql);
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return false;
  }

  if (query_result.empty()) {
    return false;
  }

  // Print objs using TextTable
  TextTable result('-', '|', '+');
  result.add(" TABLE NAME ");
  result.endOfRow();

  cout << colors::reset << "\nList of all available tables:" << endl;
  for (pqxx::result::const_iterator row = query_result.begin();
       row != query_result.end(); ++row) {
    result.add(row["tablename"].as<string>(), colors::green);
    result.endOfRow();
  }

  result.setAlignment(2, TextTable::Alignment::RIGHT);
  cout << result;
  return true;
}

bool Test_GetFunctions(shared_ptr<DBProvider> db_prov) { return false; }

bool Test_GetFunctionParameters(shared_ptr<DBProvider> db_prov) {
  return false;
}

bool Test_GetTriggers(shared_ptr<DBProvider> db_prov) { return false; }

bool Test_PrintFunctions(shared_ptr<DBProvider> db_prov) {
  std::string query_request =
      "SELECT r.routine_name ,"
      " array_to_string(p.proargnames, ', ', '*'),"
      " pg_get_functiondef(p.oid)"
      " FROM information_schema.routines r, pg_catalog.pg_proc p"
      " WHERE r.external_language = 'PLPGSQL'"
      " AND r.specific_name = p.proname || '_' || p.oid;";

  // Init text table
  TextTable text_table('-', '|', '+');

  auto query_result = db_prov->Query(query_request);

  vector<string> func_names;
  vector<string> func_params;

  map<string, vector<string>> result;

  for (auto row = query_result.begin(); row != query_result.end(); ++row) {
    auto f_name = row["routine_name"].as<string>();
    func_names.emplace_back(f_name);

    std::string f_params;

    if (!row["array_to_string"].is_null())
      f_params = row["array_to_string"].as<string>();

    func_params.emplace_back(f_params);

    auto prams_vec = ParsingTools::SplitToVector(f_params, ", ");

    result[f_name] = prams_vec;
  }

  text_table.add("  NAME  ");
  text_table.add("    PARAMS    ");
  text_table.endOfRow();

  for (auto &item : result) {
    auto str = ParsingTools::JoinAsString(item.second, ", ");
    text_table.add(item.first, colors::red);
    text_table.add(str, colors::yellow);
    text_table.endOfRow();
  }

  text_table.setAlignment(2, TextTable::Alignment::RIGHT);
  cout << text_table;
  return !result.empty();
}

bool Test_PrintFunctionParameters(shared_ptr<DBProvider> db_prov) {
  return false;
}

bool Test_PrintTriggers(shared_ptr<DBProvider> db_prov) { return false; }

bool Test_PrintObjectsAsTable(shared_ptr<DBProvider> db_prov) {
  vector<ObjectData> objects;

  try {
    // Get all objects from database
    objects = db_prov->GetObjects();
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return false;
  }

  if (objects.empty()) {
    cout << "\nCould not get objects from database." << endl;
    cout << "[No exception thrown]" << endl;
    return false;
  }

  // Print objects using TextTable
  TextTable result('-', '|', '+');
  result.add("NAME");
  result.add("TYPE");
  result.add("SCHEME");
  result.add("PARAMS");
  result.endOfRow();

  for (auto &object : objects) {
    result.add(object.name);
    result.add(CastObjectType(object.type));
    result.add(object.schema);

    string acc = "";
    if (!object.params.empty()) {
      for (const auto &param : object.params) {
        acc += param + " ";
      }
    }

    result.add(acc);
    result.endOfRow();
  }

  result.setAlignment(2, TextTable::Alignment::RIGHT);
  cout << result;
  return true;
}

bool Test_PqxxConnection() {
  try {
    pqxx::connection conn("dbname = Doors user = doo password = rs hostaddr = "
                          "127.0.0.1 port = 5432");

    if (!conn.is_open()) {
      cout << "Can't open database" << endl;
      cout << "[No exception thrown]" << endl;
      return false;
    }

    cout << "Opened database successfully: " << conn.dbname() << endl;
    cout << "Disconnecting from: " << conn.dbname() << endl;
    conn.disconnect();
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return false;
  }

  return true;
}

void RunTest_PrintScriptsText(shared_ptr<DBProvider> db_prov) {
  cout << colors::cyan << "\nRUNNING TEST: SCRIPT DEFINITIONS:" << colors::reset
       << endl;

  ObjectData test_view("pg_roles", ObjectType::View, "");
  ObjectData test_sequence("math_blob_id_seq", ObjectType::Sequence, "public");
  ObjectData test_index("math_blob_1_id_idx", ObjectType::Index, "public");
  ObjectData test_function("clear_serie", ObjectType::Function, "io",
                           {"p_serie_path"});
  ObjectData test_trigger("placeholder", ObjectType::Trigger, "public");

  list<string> objects_definitions = {
      db_prov->GetScriptData(test_function, vector<ScriptDefinition>{}).text,
      db_prov->GetScriptData(test_trigger, vector<ScriptDefinition>{}).text,
      // db_prov->GetScriptData(test_view, vector<ScriptDefinition>{}).text,
      db_prov->GetScriptData(test_index, vector<ScriptDefinition>{}).text,
      db_prov->GetScriptData(test_sequence, vector<ScriptDefinition>{}).text};

  // Print objects using TextTable
  TextTable text_table('-', '|', '+');

  for (const auto &definition : objects_definitions) {
    if (!definition.empty()) {
      text_table.add(definition);
    } else {
      text_table.add("EMPTY");
    }

    text_table.endOfRow();
  }

  text_table.setAlignment(2, TextTable::Alignment::LEFT);
  cout << text_table;
}

void PrintAllFunctionParams(shared_ptr<DBProvider> db_prov) {
  const auto query_result = db_prov->Query(
      "SELECT r.routine_name, array_to_string(p.proargnames, ', ', '*')\
								   FROM information_schema.routines r, pg_catalog.pg_proc p\
								   WHERE r.external_language = 'PLPGSQL'  AND r.routine_name = p.proname AND r.specific_name = p.proname || '_' || p.oid;");
  // Print objects using TextTable
  TextTable text_table('-', '|', '+');

  cout << colors::blue << "\nList of all function parameters:" << colors::reset
       << endl;
  for (pqxx::result::const_iterator row = query_result.begin();
       row != query_result.end(); ++row) {
    text_table.add(row["routine_name"].c_str(), colors::red);
    text_table.add(row["array_to_string"].c_str(), colors::yellow);
    text_table.endOfRow();
  }

  text_table.setAlignment(2, TextTable::Alignment::RIGHT);
  cout << text_table;
}

bool Test_Logger() {
  try {
    using namespace PatcherLogger;

    // create two different logs
    auto *test_log = new Log();

    // write some messages to both
    test_log->SetLogByPath("../build/DBProvider.dir/log_test.txt");
    test_log->AddLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
    test_log->AddLog(DEBUG, "TEST - 01");

    auto *log_in_std_dir = new Log();

    log_in_std_dir->SetLogByName("new_log.LogWithLevel");
    log_in_std_dir->AddLog(DEBUG,
                           "RUNNING: testLogger() from DBProvider_tests");
    log_in_std_dir->AddLog(DEBUG, "TEST - 02");

    // init global LogWithLevel
    StartGlobalLog("../build/DBProvider.dir/global_log.txt");

    // write some messages to global LogWithLevel
    LogDebug("Test GLOBAL LOG");

    // delete logs (not files)
    delete log_in_std_dir;
    delete test_log;
    StopGlobalLog();
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return false;
  }

  return true;
}

bool Test_CustomConnection() {
  try {
    string str = "Doors:doo:rc:127.0.0.1:5432";
    auto db_conn = make_shared<DBConnection>(str);

    cout << "hostaddr= " << db_conn->GetConnection()->hostname() << endl;

    cout << "port= " << db_conn->GetConnection()->port() << endl;

    cout << "dbname= " << db_conn->GetConnection()->dbname() << endl;

    cout << "user= " << db_conn->GetConnection()->username() << endl;

    cout << "password= " << db_conn->GetConnection()->username() << endl;

    db_conn.reset();
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return false;
  }

  return true;
}

bool Test_PrintObjectsData(shared_ptr<DBProvider> db_prov) {
  const string get_objects = "SELECT /*sequences */"
                             "f.sequence_schema AS obj_schema,"
                             "f.sequence_name AS obj_name,"
                             "'SequenceType' AS obj_type "
                             "FROM information_schema.sequences f "
                             "UNION ALL "
                             "SELECT /*tables */ "
                             "f.table_schema AS obj_schema,"
                             "f.table_name AS obj_name,"
                             "'tables' AS obj_type "
                             "FROM information_schema.tables f "
                             "WHERE f.table_schema in"
                             "('public', 'io', 'common', 'secure');";

  const auto query_result = db_prov->Query(get_objects);
  PrintObjectsData(query_result);
  return true;
}

bool Test_PrintObjectsDataShared(const shared_ptr<DBProvider> &db_prov) {
  const string get_objects = "SELECT /*sequences */"
                             "f.sequence_schema AS obj_schema,"
                             "f.sequence_name AS obj_name,"
                             "'SequenceType' AS obj_type "
                             "FROM information_schema.sequences f "
                             "UNION ALL "
                             "SELECT /*tables */ "
                             "f.table_schema AS obj_schema,"
                             "f.table_name AS obj_name,"
                             "'tables' AS obj_type "
                             "FROM information_schema.tables f "
                             "WHERE f.table_schema in"
                             "('public', 'io', 'common', 'secure');";

  const auto query_result = db_prov->Query(get_objects);
  PrintObjectsData(query_result);
  return true;
}

void RunSimpleTests() {
  const string login = "127.0.0.1:5432:Doors:doo:rs";
  auto provider = make_shared<DBProvider>(login);
  provider->Connect();

  cout << colors::yellow << "Standard Test : make sure pqxx::GetConnectionFromPool works"<< colors::reset << endl;
  cout << colors::red << Test_PqxxConnection() << colors::reset << endl;

  cout << colors::yellow << "Test PatchLogger::Log : write to different logs"<< colors::reset << endl;
  cout << colors::red << Test_Logger() << colors::reset << endl;

  cout << colors::yellow << "Test printing raw data : printing raw objects data"<< colors::reset << endl;
  cout << colors::green << Test_PrintObjectsData(provider)
       << colors::reset << endl;

  cout << colors::yellow << "Test printing data in table : printing objects data wrapped in table"
      << colors::reset << endl;
  //cout << colors::red << Test_PrintObjectsAsTable(provider) << colors::reset << endl;

  cout << colors::yellow << "Test printing all tables : printing tables in io, public, common, "
          "secure"
      << colors::reset << endl;
  cout << colors::red << Test_PrintTableList(provider)
       << colors::reset << endl;


  // check point
  bool temp = Test_prepared();
  cout << colors::yellow << temp << "Test : check table existence"<< colors::reset << endl;
  cout << colors::red << Test_tableExists(provider) << colors::reset
       << endl;

  cout << colors::yellow << "Test : check SequenceType existence"<< colors::reset << endl;
  cout << colors::red << Test_sequenceExists(provider)
       << colors::reset << endl;

  RunTest_PrintScriptsText(provider);

  PrintAllFunctionParams(provider);

  cout << colors::yellow << "Test functions"<< colors::reset << endl;
  cout << colors::red << Test_GetFunctions(provider) << colors::reset
       << endl;

  cout << colors::yellow << "Test function parameters"<< colors::reset << endl;
  cout << colors::red << Test_GetFunctionParameters(provider)
       << colors::reset << endl;

  cout << colors::yellow << "Test triggers"<< colors::reset << endl;
  cout << colors::red << Test_GetTriggers(provider) << colors::reset
       << endl;

  cout << colors::yellow << "Test printing functions"<< colors::reset << endl;
  cout << colors::red << Test_PrintFunctions(provider)
       << colors::reset << endl;

  cout << colors::yellow << "Test printing function parameters"<< colors::reset << endl;
  cout << colors::red << Test_PrintFunctionParameters(provider)
       << colors::reset << endl;

  cout << colors::yellow << "Test printing triggers"<< colors::reset << endl;
  cout << colors::red << Test_PrintTriggers(provider)
       << colors::reset << endl;
}

int main(int argc, char **argv) {
  RunSimpleTests();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
