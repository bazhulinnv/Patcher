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
		Connection() = default;

		explicit Connection(std::string& loginStringPG);

		~Connection();

		void setConnection(std::string& loginStringPG) override;

		LoginData getParameters() override;

		std::shared_ptr<pqxx::connection_base> getConnection() override;;

	private:
		void closeConnection() override;
	};
}

#endif
