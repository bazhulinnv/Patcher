#include "DBProvider/Connection.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DBConnection::Connection::Connection()
{
	db_connection_ = make_shared<pqxx::lazyconnection>();
}

DBConnection::Connection::Connection(string& pgpass_str)
{
	connection_params_ = LoginData(pgpass_str);
	parameters_set_ = true;
}

bool DBConnection::Connection::IsConnectionSet()
{
	return connection_set_ && parameters_set_;
}

bool DBConnection::Connection::IsOpen()
{
	return db_connection_->is_open();
}

DBConnection::Connection::~Connection()
{
	if (ConnectionBase::IsOpen())
	{
		Connection::CloseConnection();
	}
}

void DBConnection::Connection::SetConnection(string& pgpass_str)
{
	if (connection_set_)
	{
		Connection::CloseConnection();
	}

	connection_params_ = LoginData(pgpass_str);
	parameters_set_ = true;

	try
	{
		db_connection_ = make_shared<pqxx::lazyconnection>(connection_params_.LoginString_Pqxx());
		db_connection_->activate();
		connection_set_ = true;
	}
	catch (exception& err)
	{
		cerr << err.what() << endl;
		throw runtime_error("ERROR: Couldn't establish connection.");
	}
}

void DBConnection::Connection::SetConnection()
{
	if (!parameters_set_)
	{
		throw runtime_error("ERROR: Tried to access parameters, but parameters"
							"weren't set properly in constructor or by \"SetConnection(LoginString_PG)\".");
	}

	SetConnection(connection_params_.LoginString_PG());
}

LoginData DBConnection::Connection::GetParameters()
{
	if (!parameters_set_)
	{
		throw runtime_error(
			R"(ERROR: Tried to get parameters, but parameters weren't"
					"set properly in constructor or by "SetConnection".)");
	}

	return connection_params_;
}

shared_ptr<pqxx::connection_base> DBConnection::Connection::GetConnection()
{
	if (!connection_set_)
	{
		throw runtime_error(
			R"(ERROR: Tried to get pqxx::connection_base, but parameters weren't"
					"set properly in constructor or by "SetConnection".)");
	}

	return db_connection_;
}

void DBConnection::Connection::CloseConnection()
{
	db_connection_->disconnect();
	db_connection_.reset();
	connection_params_ = LoginData();
	parameters_set_ = false;
	connection_set_ = false;
}
