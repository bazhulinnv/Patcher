#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "DBProvider/Connection.h"
#include "DBProvider/LoginData.h"
#include <mutex>
#include <memory>
#include <deque>

namespace DBConnection
{
	class ConnectionPool
	{
	public:
		explicit ConnectionPool(std::string& pgpass_string);

		explicit ConnectionPool(std::string& pgpass_string, int size);

		void resetPoolParameters(std::string& pgpass_string);

		void resetPoolParameters(const LoginData& params);

		std::shared_ptr<Connection> connection();

		void freeConnection(std::shared_ptr<Connection> conn);

	private:
		std::mutex mtx;
		std::condition_variable poolCondition;
		std::deque<std::shared_ptr<Connection>> pool;
		int poolSize = 10;

		void createPool(std::string& pgpass_string);
	};
};

#endif