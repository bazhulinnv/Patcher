#ifndef CONNECTION_H
#define CONNECTION_H

#include "DBProvider/ConnectionBase.h"
#include "DBProvider/LoginData.h"
#include <string>
#include <memory>
#include <pqxx/pqxx>

namespace DBConnection
{
	class Connection : ConnectionBase
	{
	public:
		Connection();

		explicit Connection(std::string& pgpass_str);

		bool IsConnectionSet() override;

		bool IsOpen() override;

		~Connection();

		void SetConnection(std::string& pgpass_str) override;

		void SetConnection();

		LoginData GetParameters() override;

		std::shared_ptr<pqxx::connection_base> GetConnection() override;

	private:
		void CloseConnection() override;
	};
}

#endif
