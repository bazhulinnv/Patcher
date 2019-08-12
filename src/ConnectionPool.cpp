#include "DBProvider/ConnectionPool.h"
#include "Shared/Logger.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace DBConnectionPool
{
	ConnectionPool::ConnectionPool()
	{
		createPool();
	}

	ConnectionPool::ConnectionPool(int size)
	{
		poolSize = size;
		createPool();
	}

	void ConnectionPool::setConnections(LoginData params)
	{
		const auto connStr = params.getLoginStringPqxx();
		PatcherLogger::Log connPoolLog;
		connPoolLog.setLogByName("ConnectionPool_setConnection.log");
		connPoolLog.addLog(PatcherLogger::INFO, "Connecting through libpqxx to database with params:\n\t\t" + connStr);

		std::lock_guard<std::mutex> locker(mtx);
		for (auto& conn : connPool)
		{
			try
			{
				// Reset current connection from connection pool
				conn.reset(new pqxx::connection(connStr));
			}
			catch (std::exception& err)
			{
				// Nothing to do. Other connections may work.
				connPoolLog.addLog(PatcherLogger::WARNING, "DBConnectionPool exception: Couldn't connect to database with:\n\t\t" + connStr);
				std::clog << "DBConnectionPool exception: " << err.what() << std::endl;
				freeConnection(conn);
			}
		}

		const int actualQuantity = connPool.size();

		if (actualQuantity == 0)
		{
			connPoolLog.addLog(PatcherLogger::ERROR, "DBConnectionPool exception: ALL CONNECTIONS FAILED.");
			std::cerr << "DBConnectionPool Pool: ALL CONNECTIONS FAILED.\n Check connection parameters" << std::endl;
			throw std::invalid_argument("Couldn't connect to database with:\n\t\t" + connStr);
		}

		if (actualQuantity < poolSize)
		{
			poolSize = actualQuantity;
		}
	}

	void ConnectionPool::setConnections(const std::string& pgLogin)
	{
		setConnections(LoginData(std::move(pgLogin)));
	}

	void ConnectionPool::createPool()
	{
		std::lock_guard<std::mutex> locker(mtx);

		for (int i = 0; i < poolSize; ++i)
		{
			connPool.push_back(std::make_shared<pqxx::connection>());
		}
	}

	std::shared_ptr<pqxx::connection> ConnectionPool::connection()
	{
		std::unique_lock<std::mutex> lock(mtx);

		// if pool is empty, then wait until it notifies back
		while (connPool.empty())
		{
			poolCondition.wait(lock);
		}

		// get new connection in queue
		auto conn = connPool.front();
		// immediately pop as we will use it now
		connPool.pop_front();

		return conn;
	}

	void ConnectionPool::freeConnection(const std::shared_ptr<pqxx::connection> conn)
	{
		std::unique_lock<std::mutex> lock(mtx);

		// push a new connection into a pool
		connPool.push_back(conn);

		// unlock mutex
		lock.unlock();

		// notify one of thread that is waiting
		poolCondition.notify_one();
	}
}
