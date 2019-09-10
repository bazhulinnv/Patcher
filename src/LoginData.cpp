#include "DBProvider/LoginData.h"
#include "Shared/ParsingTools.h"
#include <iostream>
#include <stdexcept>

using namespace std;

LoginData::LoginData(string hostname_, unsigned int port_, string database_,
                     string username_, string password_) {
  // hostname:port:database:username:password -- PostgresSQL pgpass format
  hostname = move(hostname_);
  port = port_;
  database = move(database_);
  username = move(username_);
  password = move(password_);
}

LoginData::LoginData(const string &login_pgpass_string) {
  try {
  	auto values = ParsingTools::SplitToVector(login_pgpass_string, ":");
    hostname = values[0];
    port = stoi(values[1]);
    database = values[2];
    username = values[3];
    password = values[4];
  } catch (exception &err) {
    cerr << err.what() << endl;
    throw invalid_argument(
        "PARSING ERROR: Couldn't parse database connection parameters.");
  }
}

// Returns login data as single string in libpqxx format
// e.g. "hostname=127.0.0.1 port=5432 dbname=example username=user
// password=qwerty123"

string LoginData::LoginString_Pqxx() const {
  if (hostname.empty() || database.empty() || username.empty() ||
      password.empty()) {
    throw invalid_argument("ERROR: Invalid database parameters.");
  }

  const auto pgLogin = hostname + ":" + to_string(port) + ":" + database + ":" +
                       username + ":" + password;
  return ParsingTools::ParseCredentials(pgLogin);
}

// Returns login data as single string (PGPASSFILE format)
// e.g. "hostname:port:database:username:password"

string LoginData::LoginString_PG() const {
  if (hostname.empty() || database.empty() || username.empty() ||
      password.empty()) {
    throw invalid_argument("ERROR: Invalid database connection parameters.");
  }

  return hostname + ":" + to_string(port) + ":" + database + ":" + username +
         ":" + password;
}
