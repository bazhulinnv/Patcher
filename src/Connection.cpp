#include "DBProvider/Connection.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DBConnection::Connection::Connection()
{
	dbConnection = make_shared<pqxx::connection>();
}

DBConnection::Connection::Connection(string& pgpass_str)
{
	connectionParams = LoginData(pgpass_str);
	parametersSet = true;
}

DBConnection::Connection::~Connection()
{
	if (connectionSet)
	{
		Connection::closeConnection();
	}
}

void DBConnection::Connection::setConnection(string& pgpass_str)
{
	if (connectionSet)
	{
		Connection::closeConnection();
	}

	connectionParams = LoginData(pgpass_str);
	parametersSet = true;

	try
	{
		dbConnection = make_shared<pqxx::connection>(connectionParams.loginStringPqxx());
		connectionSet = true;
	}
	catch (exception& err)
	{
		cerr << err.what() << endl;
		throw runtime_error("ERROR: Couldn't establish connection.");
	}
}

void DBConnection::Connection::setConnection()
{
	if (!parametersSet)
	{
		throw runtime_error("ERROR: Tried to access parameters, but parameters"
							"weren't set properly in constructor or by \"setConnection(loginStringPG)\".");
	}

	setConnection(connectionParams.loginStringPG());
}

LoginData DBConnection::Connection::getParameters()
{
	if (!parametersSet)
	{
		throw runtime_error(
			R"(ERROR: Tried to get parameters, but parameters weren't"
					"set properly in constructor or by "setConnection".)");
	}

	return connectionParams;
}

shared_ptr<pqxx::connection_base> DBConnection::Connection::getConnection()
{
	if (!connectionSet)
	{
		throw runtime_error(
			R"(ERROR: Tried to get pqxx::connection_base, but parameters weren't"
					"set properly in constructor or by "setConnection".)");
	}

	return dbConnection;
}

void DBConnection::Connection::closeConnection()
{
	dbConnection->disconnect();
	dbConnection.reset();
	connectionParams = LoginData();
	parametersSet = false;
	connectionSet = false;
}
