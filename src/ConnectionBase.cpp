#include "DBProvider/ConnectionBase.h"

DBConnection::ConnectionBase::ConnectionBase()
{
	db_connection_ = std::make_shared<pqxx::lazyconnection>();
}

DBConnection::ConnectionBase::~ConnectionBase()
{
	db_connection_.reset();
}

bool DBConnection::ConnectionBase::IsConnectionSet()
{
	return connection_set_ && parameters_set_;
}

bool DBConnection::ConnectionBase::IsOpen()
{
	return db_connection_->is_open();
}
