#include "Shared/Logger.h"
#include "Shared/TestUtility.h"
#include "Shared/TextTable.h"
#include "Shared/ParsingTools.h"

#include "DBProvider/DBProvider.h"
#include "DBProvider/ConnectionPool.h"
#include "DBProvider/Connection.h"

#include <pqxx/pqxx>
#include <iostream>
#include <memory>
#include <functional>
#include <thread>

using namespace std;

bool Test_sequenceExists(DBProvider* db_prov)
{
	return db_prov->SequenceExists("public", "math_blob_id_seq");
}

bool Test_tableExists(DBProvider* db_prov)
{
	return db_prov->TableExists("public", "errors");
}

bool Test_PrintTableList(DBProvider* db_prov)
{
	pqxx::result query_result;

	const string request_sql = "SELECT * FROM pg_catalog.pg_tables "
		"WHERE schemaname != 'pg_catalog' "
		"AND schemaname != 'information_schema'; ";
	try
	{
		// Get all tables from database
		query_result = db_prov->Query(request_sql);
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	if (query_result.empty())
	{
		return false;
	}

	// Print objs using TextTable
	TextTable result('-', '|', '+');
	result.add(" TABLE NAME ");
	result.endOfRow();

	cout << "\nList of all available tables:" << endl;
	for (pqxx::result::const_iterator row = query_result.begin();
		 row != query_result.end(); ++row)
	{
		result.add(row["tablename"].as<string>(), green);
		result.endOfRow();
	}

	result.setAlignment(2, TextTable::Alignment::RIGHT);
	cout << result;
	return true;
}

bool Test_GetFunctions(DBProvider* db_prov)
{
	return false;
}

bool Test_GetFunctionParameters(DBProvider* db_prov)
{
	return false;
}

bool Test_GetTriggers(DBProvider* db_prov)
{
	return false;
}

bool Test_PrintFunctions(DBProvider* db_prov)
{
	std::string query_request = "SELECT r.routine_name ,"
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

	for (pqxx::result::const_iterator row = query_result.begin();
		 row != query_result.end();
		 ++row)
	{
		auto f_name = row["routine_name"].as<string>();
		func_names.emplace_back(f_name);

		std::string f_params;

		bool lala = row["array_to_string"].is_null();
		if (!lala)
		{
			f_params = row["array_to_string"].as<string>();
		}

		func_params.emplace_back(f_params);

		auto prams_vec = ParsingTools::SplitToVector(f_params, ", ");

		result[f_name] = prams_vec;
	}

	text_table.add("  NAME  ");
	text_table.add("    PARAMS    ");
	text_table.endOfRow();

	for (auto& item : result)
	{
		auto str = ParsingTools::JoinAsString(item.second, ", ");
		text_table.add(item.first, red);
		text_table.add(str, yellow);
		text_table.endOfRow();
	}

	text_table.setAlignment(2, TextTable::Alignment::RIGHT);
	cout << text_table;
	return !result.empty();
}

bool Test_PrintFunctionParameters(DBProvider* db_prov)
{
	return false;
}

bool Test_PrintTriggers(DBProvider* db_prov)
{
	return false;
}

bool Test_PrintObjectsAsTable(DBProvider* db_prov)
{
	vector<ObjectData> objects;

	try
	{
		// Get all objects from database
		objects = db_prov->GetObjects();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	if (objects.empty())
	{
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

	for (auto& object : objects)
	{
		result.add(object.name);
		result.add(object.type);
		result.add(object.schema);

		string acc = "";
		if (!object.params.empty())
		{
			for (const auto& param : object.params)
			{
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

bool Test_PqxxConnection()
{
	try
	{
		pqxx::connection conn("dbname = Doors user = doo password = rs hostaddr = 127.0.0.1 port = 5432");

		if (!conn.is_open())
		{
			cout << "Can't open database" << endl;
			cout << "[No exception thrown]" << endl;
			return false;
		}

		cout << "Opened database successfully: " << conn.dbname() << endl;
		cout << "Disconnecting from: " << conn.dbname() << endl;
		conn.disconnect();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

void RunConnectionPool(const shared_ptr<DBConnection::ConnectionPool>& pg_pool, size_t id)
{
	auto conn = pg_pool->GetConnectionFromPool();
	string request_sql = "SELECT * FROM pg_catalog.pg_tables "
		"WHERE schemaname != 'pg_catalog' "
		"AND schemaname != 'information_schema'; ";
	try
	{
		// create a transaction from a GetConnectionFromPool
		pqxx::work work(reinterpret_cast<pqxx::connection&>(*conn->GetConnection()));
		pqxx::result query_result = work.exec(request_sql);

		// Print objects using TextTable
		TextTable result('-', '|', '+');
		result.add(" TABLE NAME ");
		result.endOfRow();

		cout << "\nList of all available tables:" << endl;
		for (pqxx::result::const_iterator row = query_result.begin();
			 row != query_result.end(); ++row)
		{
			result.add(row["tablename"].as<string>(), green);
			result.endOfRow();
		}

		result.setAlignment(2, TextTable::Alignment::RIGHT);
		cout << result;
		cout << "ID: " << id << endl;
		work.commit();

		// free GetConnectionFromPool when things done
		pg_pool->FreeConnection(conn);
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		// free GetConnectionFromPool when error kicks in
		pg_pool->FreeConnection(conn);
	}
}

void TestConnectionPool_MultiThreading(string& pg_login)
{
	auto pg_pool = make_shared<DBConnection::ConnectionPool>(pg_login);
	vector<shared_ptr<thread>> vec;

	// number of thread here should be number of thread available in your thread pool waiting for incoming job to execute
	// imagine X threads are waiting to execute task
	for (size_t i = 0; i < 10; i++)
	{
		vec.push_back(make_shared<thread>(thread(RunConnectionPool, pg_pool, i)));
	}

	for (auto& i : vec)
	{
		i.get()->join();
	}
}

void PrintAllFunctionParams(DBProvider* db_prov)
{
	const auto query_result = db_prov->Query("SELECT r.routine_name, array_to_string(p.proargnames, ', ', '*')\
								   FROM information_schema.routines r, pg_catalog.pg_proc p\
								   WHERE r.external_language = 'PLPGSQL'  AND r.routine_name = p.proname AND r.specific_name = p.proname || '_' || p.oid;");
	// Print objects using TextTable
	TextTable text_table('-', '|', '+');

	cout << blue << "\nList of all function parameters:" << reset << endl;
	for (pqxx::result::const_iterator row = query_result.begin();
		 row != query_result.end();
		 ++row)
	{
		text_table.add(row["routine_name"].c_str(), red);
		text_table.add(row["array_to_string"].c_str(), yellow);
		text_table.endOfRow();
	}

	text_table.setAlignment(2, TextTable::Alignment::RIGHT);
	cout << text_table;
}

bool Test_Logger()
{
	try
	{
		using namespace PatcherLogger;

		// create two different logs
		auto* test_log = new Log();

		// write some messages to both
		test_log->SetLogByPath("../build/DBProvider.dir/log_test.txt");
		test_log->AddLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		test_log->AddLog(DEBUG, "TEST - 01");

		auto* log_in_std_dir = new Log();

		log_in_std_dir->SetLogByName("new_log.LogWithLevel");
		log_in_std_dir->AddLog(DEBUG, "RUNNING: testLogger() from DBProvider_tests");
		log_in_std_dir->AddLog(DEBUG, "TEST - 02");

		// init global LogWithLevel 
		StartGlobalLog("../build/DBProvider.dir/global_log.txt");

		// write some messages to global LogWithLevel
		LogDebug("Test GLOBAL LOG");

		// delete logs (not files)
		delete log_in_std_dir;
		delete test_log;
		StopGlobalLog();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

bool Test_CustomConnection()
{
	try
	{
		string str = "Doors:doo:rc:127.0.0.1:5432";
		auto db_conn = make_shared<DBConnection::Connection>(str);

		cout << "hostaddr= "
			<< db_conn->GetConnection()->hostname() << endl;

		cout << "port= "
			<< db_conn->GetConnection()->port() << endl;

		cout << "dbname= "
			<< db_conn->GetConnection()->dbname() << endl;

		cout << "user= "
			<< db_conn->GetConnection()->username() << endl;

		cout << "password= "
			<< db_conn->GetConnection()->username() << endl;

		db_conn.reset();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

bool Test_PrintObjectsData(DBProvider* db_prov)
{
	const string get_objects =
		"SELECT /*sequences */"
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
		"('public', 'io', 'common', 'secure');";

	const auto query_result = db_prov->Query(get_objects);
	PrintObjectsData(query_result);
	return true;
}

bool Test_PrintObjectsDataShared(const shared_ptr<DBProvider>& db_prov)
{
	const string get_objects =
		"SELECT /*sequences */"
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
		"('public', 'io', 'common', 'secure');";

	const auto query_result = db_prov->Query(get_objects);
	PrintObjectsData(query_result);
	return true;
}

int main()
{
	using namespace TestUtility;
	string sql_public_role_action = "SELECT * FROM public.role_action";

	string login_data = "127.0.0.1:5432:Doors:doo:rs";
	auto* db_provider = new DBProvider(login_data);

	// < test_info, pointer_to_test_function >
	vector<pair<const string, function<bool()>>> simple_tests;
	simple_tests.emplace_back("Standard Test : make sure pqxx::GetConnectionFromPool works",
							  Test_PqxxConnection);

	simple_tests.emplace_back("Test PatchLogger::Log : write to different logs",
							  Test_Logger);
	/*simpleTests.push_back(make_pair("Test Connection : connect to 'Doors'", Test_CustomConnection));*/

	vector<pair<const string, function<bool(DBProvider*)>>> provider_test;
	vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>> provider_tests_shared;

	provider_test.emplace_back("Test printing raw data : printing raw objects data",
							   Test_PrintObjectsData);

	provider_test.emplace_back("Test printing data in table : printing objects data wrapped in table",
							   Test_PrintObjectsAsTable);

	provider_test.emplace_back("Test printing all tables : printing tables in io, public, common, secure",
							   Test_PrintTableList);

	provider_test.emplace_back("Test : check table existence",
							   Test_tableExists);

	provider_test.emplace_back("Test : check sequence existence",
							   Test_sequenceExists);

	//TestConnectionPool_MultiThreading(loginData);

	PrintAllFunctionParams(db_provider);

	provider_test.emplace_back("Test functions",
							   Test_GetFunctions);

	provider_test.emplace_back("Test function parameters",
							   Test_GetFunctionParameters);

	provider_test.emplace_back("Test triggers",
							   Test_GetTriggers);

	provider_test.emplace_back("Test printing functions",
							   Test_PrintFunctions);

	provider_test.emplace_back("Test printing function parameters",
							   Test_PrintFunctionParameters);

	provider_test.emplace_back("Test printing triggers",
							   Test_PrintTriggers);

	// Run all simple test functions
	RunSimpleTests(simple_tests);

	// Run all test functions for DBProvider
	RunAll(provider_test, db_provider);

	return 0;
}
