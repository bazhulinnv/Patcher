#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "DBProvider/LoginData.h"

#include <mutex>
#include <pqxx/pqxx>
#include <memory>
#include <deque>
#include <string>

namespace DBConnectionPool
{
	class ConnectionPool
	{
	public:
		ConnectionPool();
		ConnectionPool(int size);

		void setConnections(LoginData params);
		void setConnections(const std::string& pgLogin);

		std::shared_ptr<pqxx::connection> connection();
		void freeConnection(std::shared_ptr<pqxx::connection>);

	private:
		void createPool();

		std::mutex mtx;
		std::condition_variable poolCondition;
		std::deque<std::shared_ptr<pqxx::connection>> connPool;
		int poolSize = 10;
	};
};

#endif