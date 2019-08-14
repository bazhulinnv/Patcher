#include "DBProvider/ConnectionBase.h"

bool DBConnection::ConnectionBase::isConnectionSet()
{
	return connectionSet && parametersSet;
}

bool DBConnection::ConnectionBase::isOpen()
{
	return dbConnection->is_open();
}
