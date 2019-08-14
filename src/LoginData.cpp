#include "DBProvider/LoginData.h"
#include "Shared/ParsingTools.h"

#include <iostream>
#include <stdexcept>

using namespace std;

LoginData::LoginData(string _hostname, unsigned int _port, string _database, string _username, string _password)
{
	// hostname:port:database:username:password -- PostgresSQL pgpass format
	hostname = move(_hostname);
	port = _port;
	database = move(_database);
	username = move(_username);
	password = move(_password);
}

LoginData::LoginData(const string& login_string_pg)
{
	vector<string> values;

	try
	{
		values = ParsingTools::splitToVector(login_string_pg, ":");
	}
	catch (exception& err)
	{
		cerr << err.what() << endl;
		throw invalid_argument("PARSING ERROR: Couldn't parse database connection parameters.");
	}

	hostname = values[0];
	port = stoi(values[1]);
	database = values[2];
	username = values[3];
	password = values[4];
}

// Returns login data as single string in libpqxx format
// e.g. "hostname=127.0.0.1 port=5432 dbname=example username=user password=qwerty123"

string LoginData::loginStringPqxx() const
{
	if (hostname.empty() || database.empty() || username.empty() || password.empty())
	{
		throw invalid_argument("ERROR: Invalid database parameters.");
	}

	const auto pgLogin = hostname + ":" + to_string(port) + ":" + database + ":" + username + ":" + password;
	return ParsingTools::parseCredentials(pgLogin);
}

// Returns login data as single string (PGPASSFILE format)
// e.g. "hostname:port:database:username:password"

string LoginData::loginStringPG() const
{
	if (hostname.empty() || database.empty() || username.empty() || password.empty())
	{
		throw invalid_argument("ERROR: Invalid database connection parameters.");
	}

	return hostname + ":" + to_string(port) + ":" + database + ":" + username + ":" + password;
}
