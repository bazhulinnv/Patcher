#include "Shared/TestUtility.h"
#include "Shared/TextTable.h"
#include <iostream>
#include <utility>

using namespace std;

void TestUtility::RunTest(const string& test_info, const function<bool(DBProvider*)> sut, DBProvider* db_prov)
{
	cout << yellow;
	cout << "\nRUNNING: " << reset << test_info << endl;

	if (!sut(db_prov))
	{
		cout << red;
		cout << "FAILED: ";
		cout << yellow;
		cout << test_info << reset << endl;
		return;
	}

	cout << green;
	cout << "SUCCEEDED: " << reset;
	cout << test_info << endl;
	cout << endl;
}

void TestUtility::RunTest(const string& test_info, const function<bool(shared_ptr<DBProvider>)>& sut, shared_ptr<DBProvider> db_prov)
{
	cout << yellow;
	cout << "\nRUNNING: " << reset << test_info << endl;

	if (!sut(std::move(db_prov)))
	{
		cout << red;
		cout << "FAILED: ";
		cout << yellow;
		cout << test_info << reset << endl;
		return;
	}

	cout << green;
	cout << "SUCCEEDED: " << reset;
	cout << test_info << endl;
	cout << endl;
}

void TestUtility::RunAll(const vector<pair<const string, function<bool(DBProvider*)>>>& provider_tests, DBProvider* db_prov)
{
	cout << yellow;
	cout << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : provider_tests)
	{
		RunTest(test.first, test.second, db_prov);
	}

	cout << yellow;
	cout << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
}

void TestUtility::RunAll(const vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>>& provider_tests, const shared_ptr<DBProvider>& db_prov)
{
	cout << yellow;
	cout << "\t##########\t" << "\tTESTING STARTED\t" << "\t##########" << reset << endl;

	for (auto& test : provider_tests)
	{
		RunTest(test.first, test.second, db_prov);
	}

	cout << yellow;
	cout << "\t##########\t" << "\tTESTING FINISHED.\t" << "\t##########" << reset << endl;
}

void TestUtility::RunSimpleTests(const vector<pair<const string, function<bool()>>>& simple_tests)
{
	cout << yellow;
	cout << "\t##########\t" << "RUNNING SIMPLE TESTS" << "\t##########" << reset << endl;

	for (auto& test : simple_tests)
	{
		cout << yellow;
		cout << "\nRUNNING: " << reset << test.first << endl;

		if (!test.second())
		{
			cout << red;
			cout << "FAILED: ";
			cout << yellow;
			cout << test.first << reset << endl;
			return;
		}

		cout << green;
		cout << "SUCCEEDED: " << reset;
		cout << test.first << endl;
		cout << endl;
	}

	cout << yellow;
	cout << "\t##########\t" << "FINISHED SIMPLE TESTS." << "\t##########" << reset << endl;
}
