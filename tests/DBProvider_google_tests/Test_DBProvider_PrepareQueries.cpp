#include <DBProvider/DBProvider.h>
#include <gtest/gtest.h> // Google Test Framework

using namespace Provider;
using namespace std;

TEST(PrepareQueries_tests, test_sequence_existence_query) {
  DBProvider prov;
  EXPECT_NO_THROW({
    prov.SetConnection(string("localhost:5432:Doors:doo:rs"));
    prov.Connect();
  });
  ASSERT_TRUE(prov.SequenceExists("public", "math_blob_id_seq"));
}

TEST(PrepareQueries_tests, test_function_existence_query) {
  DBProvider prov;
  EXPECT_NO_THROW({
    prov.SetConnection(string("localhost:5432:Doors:doo:rs"));
    prov.Connect();
  });
  auto statements = prov.GetPreparedStatements();
  auto function_exists_statement = statements["function_exists"];
  ASSERT_TRUE(prov.FunctionExists("io", "get_series()"));
}