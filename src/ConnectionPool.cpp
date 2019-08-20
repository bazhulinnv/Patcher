#include "DBProvider/ConnectionPool.h"
#include "Shared/Logger.h"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace DBConnection
{
	ConnectionPool::ConnectionPool(string& pgpass_string)
	{
		CreatePool(pgpass_string);
	}

	ConnectionPool::ConnectionPool(string& pgpass_string, const int size)
	{
		pool_size_ = size;
		CreatePool(pgpass_string);
	}

	void ConnectionPool::ResetPoolParameters(string& pgpass_string)
	{
		void resetPoolParameters(LoginData(pgpass_string));
	}

	void ConnectionPool::ResetPoolParameters(const LoginData& params)
	{
		PatcherLogger::Log conn_pool_log;
		conn_pool_log.SetLogByName("ConnectionPool_setConnection.log");
		conn_pool_log.AddLog(PatcherLogger::INFO, "Connecting through libpqxx to database with params:\n\t\t" + params.LoginString_Pqxx());

		lock_guard<mutex> locker(mtx);
		for (auto& conn : pool_)
		{
			try
			{
				// Reset current GetConnectionFromPool from GetConnectionFromPool pool
				conn->SetConnection(params.LoginString_PG());
			}
			catch (exception& err)
			{
				// Nothing to do. Other connections may work.
				conn_pool_log.AddLog(PatcherLogger::WARNING, "DBConnectionPool exception: Couldn't connect to database with:\n\t\t" + params.LoginString_Pqxx());
				clog << "DBConnectionPool exception: " << err.what() << endl;
				FreeConnection(conn);
			}
		}

		const int actual_quantity = pool_.size();

		if (actual_quantity == 0)
		{
			conn_pool_log.AddLog(PatcherLogger::ERROR, "DBConnectionPool exception: ALL CONNECTIONS FAILED.");
			cerr << "DBConnectionPool Pool: ALL CONNECTIONS FAILED.\n Check GetConnectionFromPool parameters" << endl;
			throw invalid_argument("Couldn't connect to database with:\n\t\t" + params.LoginString_Pqxx());
		}

		if (actual_quantity < pool_size_)
		{
			pool_size_ = actual_quantity;
		}
	}

	shared_ptr<Connection> ConnectionPool::GetConnectionFromPool()
	{
		unique_lock<mutex> lock(mtx);

		// if pool is empty, then wait until it notifies back
		while (pool_.empty())
		{
			pool_condition_.wait(lock);
		}

		// get new GetConnectionFromPool in queue
		auto conn = pool_.front();
		// immediately pop as we will use it now
		pool_.pop_front();

		return conn;
	}

	void ConnectionPool::FreeConnection(const shared_ptr<Connection> conn)
	{
		unique_lock<mutex> lock(mtx);

		// push a new GetConnectionFromPool into a pool
		pool_.push_back(conn);

		// unlock mutex
		lock.unlock();

		// notify one of thread that is waiting
		pool_condition_.notify_one();
	}

	void ConnectionPool::CreatePool(string& pgpass_string)
	{
		lock_guard<mutex> locker(mtx);
		for (int i = 0; i < pool_size_; ++i)
		{
			try
			{
				auto conn = make_shared<Connection>(pgpass_string);
				conn->SetConnection();
				pool_.emplace_back(conn);
			}
			catch (exception& err)
			{
				cerr << "CONNECTION POOL: Connection failed, params" << pgpass_string << endl;
				cerr << "\t\t" << err.what() << endl;
			}
		}

		if (pool_.empty())
		{
			throw invalid_argument("CONENECTION POOL: ALL CONNECTIONS FAILED.");
		}
	}
}
