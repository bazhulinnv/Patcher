#include "DBProvider/ConnectionPool.h"
#include "Shared/Logger.h"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace DBConnection
{
	ConnectionPool::ConnectionPool(string& pgpass_string)
	{
		createPool(pgpass_string);
	}

	ConnectionPool::ConnectionPool(string& pgpass_string, const int size)
	{
		poolSize = size;
		createPool(pgpass_string);
	}

	void ConnectionPool::resetPoolParameters(string& pgpass_string)
	{
		void resetPoolParameters(LoginData(pgpass_string));
	}

	void ConnectionPool::resetPoolParameters(const LoginData& params)
	{
		PatcherLogger::Log connPoolLog;
		connPoolLog.setLogByName("ConnectionPool_setConnection.log");
		connPoolLog.addLog(PatcherLogger::INFO, "Connecting through libpqxx to database with params:\n\t\t" + params.loginStringPqxx());

		lock_guard<mutex> locker(mtx);
		for (auto& conn : pool)
		{
			try
			{
				// Reset current connection from connection pool
				conn->setConnection(params.loginStringPG());
			}
			catch (exception& err)
			{
				// Nothing to do. Other connections may work.
				connPoolLog.addLog(PatcherLogger::WARNING, "DBConnectionPool exception: Couldn't connect to database with:\n\t\t" + params.loginStringPqxx());
				clog << "DBConnectionPool exception: " << err.what() << endl;
				freeConnection(conn);
			}
		}

		const int actualQuantity = pool.size();

		if (actualQuantity == 0)
		{
			connPoolLog.addLog(PatcherLogger::ERROR, "DBConnectionPool exception: ALL CONNECTIONS FAILED.");
			cerr << "DBConnectionPool Pool: ALL CONNECTIONS FAILED.\n Check connection parameters" << endl;
			throw invalid_argument("Couldn't connect to database with:\n\t\t" + params.loginStringPqxx());
		}

		if (actualQuantity < poolSize)
		{
			poolSize = actualQuantity;
		}
	}

	shared_ptr<Connection> ConnectionPool::connection()
	{
		unique_lock<mutex> lock(mtx);

		// if pool is empty, then wait until it notifies back
		while (pool.empty())
		{
			poolCondition.wait(lock);
		}

		// get new connection in queue
		auto conn = pool.front();
		// immediately pop as we will use it now
		pool.pop_front();

		return conn;
	}

	void ConnectionPool::freeConnection(shared_ptr<Connection> conn)
	{
		unique_lock<mutex> lock(mtx);

		// push a new connection into a pool
		pool.push_back(conn);

		// unlock mutex
		lock.unlock();

		// notify one of thread that is waiting
		poolCondition.notify_one();
	}

	void ConnectionPool::createPool(string& pgpass_string)
	{
		lock_guard<mutex> locker(mtx);
		for (int i = 0; i < poolSize; ++i)
		{
			try
			{
				auto conn = make_shared<Connection>(pgpass_string);
				conn->setConnection();
				pool.emplace_back(conn);
			}
			catch (exception& err)
			{
				cerr << "CONNECTION POOL: Connection failed, params" << pgpass_string << endl;
				cerr << "\t\t" << err.what() << endl;
			}
		}

		if (pool.empty())
		{
			throw invalid_argument("CONENECTION POOL: ALL CONNECTIONS FAILED.");
		}
	}
}
