#include <gtest/gtest.h> // Google Test Framework
#include <pqxx/pqxx>
#include <iostream>
#include "DBProvider/DBConnection.h"
using namespace std;

TEST(DBConnection_tests, test_good_dbconnection) {
  auto connection =
      make_unique<DBConnection>(std::string("localhost:5432:Doors:doo:rs"));

  ASSERT_TRUE(connection->IsParametersSet());
  ASSERT_FALSE(connection->IsOpen());

  connection->Connect();
  ASSERT_TRUE(connection->IsOpen());
}

TEST(DBConnection_tests, test_wrong_parameters) {
  auto connection =
      make_unique<DBConnection>(string("localhost:5432:Doors:doo:rkkkkks"));
  EXPECT_THROW(connection->Connect(), std::runtime_error);
  EXPECT_THROW(connection->GetConnection(), runtime_error);
}

TEST(DBConnection_tests, test_parameters_were_not_set) {
  auto connection = make_unique<DBConnection>();
  EXPECT_THROW(connection->Connect(), runtime_error);
}

TEST(DBConnection_tests, test_set_connection) {
  auto connection = make_unique<DBConnection>();
  EXPECT_NO_THROW(
      connection->SetConnection(string("localhost:5432:Doors:doo:rs")));
  EXPECT_NO_THROW(connection->Connect());
  EXPECT_NO_THROW(cout << connection->GetParameters().LoginString_PG() << endl);
}