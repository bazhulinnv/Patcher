#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "DBProvider/LoginData.h"

#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <stdexcept>
#include <pqxx/pqxx>

using namespace std;
class DBConnection
{
public:
	DBConnection() = default;

	explicit DBConnection(std::string& pgLogin)
	{
		connParams = LoginData(pgLogin);
	}

	~DBConnection() = default;

	std::string getConnectionString()
	{
		return connParams.getLoginStringPqxx();
	}

	LoginData getConnectionParameters()
	{
		return connParams;
	}

	void setConnection()
	{
		auto params = connParams.getLoginStringPqxx();

		try
		{
			cout << "";
		}
		catch (const std::exception& err)
		{
			cerr << "ERROR: Could not establish connection." << endl;
			cerr << "Parameters: " << params << endl;
			cerr << "ERROR Details: " << err.what() << endl;
			throw std::invalid_argument("Wrong connection parameters.\n");
		}
	}

private:
	// Shared libpqxx connection
	std::shared_ptr<pqxx::connection> *conn;

	// Stores current connection parameters
	LoginData connParams;
};

#endif