#ifndef TESTUTILITY_H
#define TESTUTILITY_H

#include "DBProvider/DBProvider.h"
#include <functional>
#include <memory>

namespace TestUtility
{
	void RunTest(const string& test_info, function<bool(DBProvider*)> sut, DBProvider* db_prov);

	void RunTest(const string& test_info, const function<bool(shared_ptr<DBProvider>)>& sut, shared_ptr<DBProvider> db_prov);

	void RunAll(const vector<pair<const string, function<bool(DBProvider*)>>>& provider_tests, DBProvider* db_prov);

	void RunAll(const vector<pair<const string, function<bool(shared_ptr<DBProvider>)>>>& provider_tests, const shared_ptr<DBProvider>& db_prov);

	void RunSimpleTests(const vector<pair<const string, function<bool()>>>& simple_tests);
}

#endif // TESTUTILITY_H
