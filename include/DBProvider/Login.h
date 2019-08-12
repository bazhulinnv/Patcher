#ifndef LOGINDATA_H
#define LOGINDATA_H

#include <string>

// Keeps login credentials
struct LoginData
{
	std::string hostname;
	int port = 5432;
	std::string database;
	std::string username;
	std::string password;

	LoginData() = default;

	explicit LoginData(std::string _hostname, unsigned int _port, std::string _database, std::string _username, std::string _password);
	
	explicit LoginData(const std::string pgLogin);

	// Returns login data as single string in libpqxx format
	// e.g. "hostname=127.0.0.1 port=5432 dbname=example username=user password=qwerty123"
	std::string getLoginStringPqxx();

	// Returns login data as single string (PGPASSFILE format)
	// e.g. "hostname:port:database:username:password"
	std::string getLoginStringPg();
};

#endif