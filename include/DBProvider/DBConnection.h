#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "DBProvider/LoginData.h"

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
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

	~DBConnection()
	{
		freeConnection();
	};

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
			std::unique_lock<std::mutex> lock_(m_mutex);
			conn = std::make_shared<pqxx::lazyconnection>(params);
		}
		catch (const std::exception& err)
		{
			cerr << "ERROR: Could not establish connection." << endl;
			cerr << "Parameters: " << params << endl;
			cerr << "ERROR Details: " << err.what() << endl;
			throw std::invalid_argument("Wrong connection parameters.\n");
		}
	}

	std::shared_ptr<pqxx::lazyconnection> getConnection()
	{
		return conn;
	}

	void freeConnection()
	{
	}

	bool isActive()
	{
		return false;
	}

private:
	// Shared libpqxx connection
	std::shared_ptr<pqxx::lazyconnection> conn;
	std::mutex m_mutex;
};

#endif