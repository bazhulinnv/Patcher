#include "Shared/TestUtility.h"
#include "Shared/TextTable.h"
#include <iostream>
#include <utility>

using namespace std;

void TestUtility::RunTest(const string& test_info, const function<bool(DBProvider*)> sut, DBProvider* db_prov)
{
	cout << yellow << "\nRUNNING: " << reset << test_info << endl;

	if (!sut(db_prov))
	{
		cout << red << "FAILED: " << yellow << test_info << reset << endl;
		return;
	}

	cout << green << "SUCCEEDED: " << reset << test_info << endl << endl;
}

void TestUtility::RunTest(const string& test_info, const function<bool(shared_ptr<DBProvider>)>& sut, shared_ptr<DBProvider> db_prov)
{
	cout << yellow << "\nRUNNING: " << reset << test_info << endl;

	if (!sut(std::move(db_prov)))
	{
		cout << red << "FAILED: " << yellow << test_info << reset << endl;
		return;
	}

	cout << green << "SUCCEEDED: " << reset << test_info << endl << endl;
}

void TestUtility::RunAll(const vector<pair<const string, function<bool(DBProvider*)>>>& provider_tests, DBProvider* db_prov)
{
	cout << yellow << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : provider_tests)
	{
		RunTest(test.first, test.second, db_prov);
	}

	cout << yellow << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
}

void TestUtility::RunAll(const vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>>& provider_tests, const shared_ptr<DBProvider>& db_prov)
{
	cout << yellow << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : provider_tests)
	{
		RunTest(test.first, test.second, db_prov);
	}

	cout << yellow << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
}

void TestUtility::RunSimpleTests(const vector<pair<const string, function<bool()>>>& simple_tests)
{
	cout << yellow << "\t##########\t" << "RUNNING SIMPLE TESTS" << "\t##########" << reset << endl;

	for (auto& test : simple_tests)
	{
		cout << yellow << "\nRUNNING: " << reset << test.first << endl;

		if (!test.second())
		{
			cout << red << "FAILED: " << yellow << test.first << reset << endl;
			return;
		}

		cout << green << "SUCCEEDED: " << reset << test.first << endl << endl;
	}

	cout << yellow << "\t##########\t" << "FINISHED SIMPLE TESTS." << "\t##########" << reset << endl;
}
