#include "DBProvider/LoginData.h"
#include "Shared/ParsingTools.h"

using namespace std;

LoginData::LoginData(std::string _hostname, unsigned int _port, std::string _database, std::string _username, std::string _password)
{
	// hostname:port:database:username:password -- PostgresSQL pgpass format
	hostname = move(_hostname);
	port = _port;
	database = move(_database);
	username = move(_username);
	password = move(_password);
}

LoginData::LoginData(const std::string& pgLogin)
{
	auto values = ParsingTools::splitToVector(pgLogin, ":");
	hostname = values[0];
	port = std::stoi(values[1]);
	database = values[2];
	username = values[3];
	password = values[4];
}

// Returns login data as single string in libpqxx format
// e.g. "hostname=127.0.0.1 port=5432 dbname=example username=user password=qwerty123"

std::string LoginData::getLoginStringPqxx() const
{
	const auto pgLogin = hostname + ":" + to_string(port) + ":" + database + ":" + username + ":" + password;
	return ParsingTools::parseCredentials(pgLogin);
}

// Returns login data as single string (PGPASSFILE format)
// e.g. "hostname:port:database:username:password"

std::string LoginData::getLoginStringPg() const
{
	return hostname + ":" + to_string(port) + ":" + database + ":" + username + ":" + password;
}
