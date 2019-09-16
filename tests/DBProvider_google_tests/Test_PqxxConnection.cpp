#include <gtest/gtest.h> // Google Test Framework
#include <iostream>
#include <pqxx/pqxx>

using namespace std;

TEST(PQXX_connection_tests, test_good_pqxx_connection) {
  pqxx::connection connection("dbname = Doors "
                              "user = doo "
                              "password = rs "
                              "hostaddr = 127.0.0.1 "
                              "port = 5432");

  ASSERT_TRUE(connection.is_open());
  cout << "Opened database successfully: " << connection.dbname() << endl;
  cout << "Disconnecting from: " << connection.dbname() << endl;
  connection.disconnect();
}

TEST(PQXX_connection_tests, test_broken_pqxx_connection) {
  EXPECT_THROW(pqxx::connection connection("dbname = Doors "
                                           "user = doo "
                                           "password = wrong-password "
                                           "hostaddr = 127.0.0.1 "
                                           "port = 5432"),
               pqxx::broken_connection);
}

TEST(PQXX_connection_tests, test_broken_pqxx_lazyconnection) {
  pqxx::lazyconnection connection("dbname = D00oors "
                                  "user = doo "
                                  "password = rs "
                                  "hostaddr = 127.0.0.1 "
                                  "port = 5432");
  EXPECT_THROW(connection.activate(), pqxx::broken_connection);
}