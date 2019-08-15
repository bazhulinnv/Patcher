#include "DBProvider/ConnectionBase.h"

DBConnection::ConnectionBase::ConnectionBase()
{
	dbConnection = std::make_shared<pqxx::connection>();
}

DBConnection::ConnectionBase::~ConnectionBase()
{
	dbConnection.reset();
}

bool DBConnection::ConnectionBase::isConnectionSet()
{
	return connectionSet && parametersSet;
}

bool DBConnection::ConnectionBase::isOpen()
{
	return dbConnection->is_open();
}
