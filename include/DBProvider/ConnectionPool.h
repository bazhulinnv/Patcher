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

		static void ResetPoolParameters(std::string& pgpass_string);

		void ResetPoolParameters(const LoginData& params);

		std::shared_ptr<Connection> GetConnectionFromPool();

		void FreeConnection(std::shared_ptr<Connection> conn);

	private:
		std::mutex mtx;
		std::condition_variable pool_condition_;
		std::deque<std::shared_ptr<Connection>> pool_;
		int pool_size_ = 10;

		void CreatePool(std::string& pgpass_string);
	};
};

#endif