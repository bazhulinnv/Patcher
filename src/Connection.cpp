#include "DBProvider/Connection.h"
#include <stdexcept>
#include <iostream>

DBConnection::Connection::Connection(std::string& loginStringPG)
{
	connectionParams = LoginData(loginStringPG);
	parametersSet = true;
}

DBConnection::Connection::~Connection()
{
	Connection::closeConnection();
}

void DBConnection::Connection::setConnection(std::string& loginStringPG)
{
	if (connectionSet)
	{
		closeConnection();
	}

	connectionParams = LoginData(loginStringPG);
	parametersSet = true;

	try
	{
		dbConnection = std::make_shared<pqxx::connection>(connectionParams.loginStringPqxx());
		connectionSet = true;
	}
	catch (std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		throw std::runtime_error("ERROR: Couldn't establish connection.");
	}
}

LoginData DBConnection::Connection::getParameters()
{
	if (parametersSet)
	{
		throw std::runtime_error(
			R"(ERROR: Tried to get parameters, but parameters weren't"
										 R"set properly in constructor or by "setConnection".)");
	}

	return connectionParams;
}

std::shared_ptr<pqxx::connection_base> DBConnection::Connection::getConnection()
{
	if (connectionSet)
	{
		throw std::runtime_error(
			R"(ERROR: Tried to get pqxx::connection_base, but parameters weren't"
										 R"set properly in constructor or by "setConnection".)");
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
