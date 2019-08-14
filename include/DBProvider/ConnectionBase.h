#ifndef DBCONNECTIONBASE_H
#define DBCONNECTIONBASE_H

#include "DBProvider/LoginData.h"
#include <pqxx/pqxx>
#include <string>

namespace DBConnection
{
	class ConnectionBase
	{
	public:
		ConnectionBase() = default;

		virtual ~ConnectionBase();

		virtual void setConnection(std::string& parameters) = 0;

		virtual bool isConnectionSet();

		virtual bool isOpen();

		virtual LoginData getParameters() = 0;

		virtual std::shared_ptr<pqxx::connection_base> getConnection() = 0;

	protected:
		bool parametersSet = false;
		bool connectionSet = false;
		LoginData connectionParams;
		std::shared_ptr<pqxx::connection_base> dbConnection;

	private:
		virtual void closeConnection() = 0;
	};
}

#endif