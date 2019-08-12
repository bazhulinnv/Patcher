#ifndef COONNECTION_POOL_H
#define COONNECTION_POOL_H

#include <pqxx/connection>

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

namespace dbpg
{
	struct LoginData
	{
		std::string host = "";
		int port = 5432;
		std::string db_name = "";
		std::string user_name = "";
		std::string user_password = "";
		int num_connection = 30;

		LoginData(std::string host_, int port_, std::string name_, std::string user_name_, std::string user_pass_, int num_con_)
		{
			: host(std::move(host_)), port(port_), db_name(std::move(name_)),
				user_name(std::move(user_name_)), user_password(std::move(user_pass_)),
				num_connection(num_con_)
		}
	};

	class ConnectionsPool
	{
	public:
		using Queries = std::map<std::string, std::string>;
		using ConnectionPointer = std::shared_ptr<pqxx::connection>;

		class Helper
		{
		public:
			Helper()
			{
				connection = ConnectionsPool::instance().connect();
			}
			
			~Helper()
			{
				ConnectionsPool::instance().freeConnect(connection);
			}

			pqxx::connection& operator*()
			{
				return *connection;
			}

		private:
			ConnectionPointer connection;
		};

		static void setParameters(LoginData& params)
		{
			if (isConnected)
			{
				throw std::runtime_error(
					"Connection pool have been started. Call \"setParams\" "
					"before first call \"instance\".");
			}
			if (params.db_name.empty() || params.host.empty() ||
				params.num_connection <= 0 || params.user_name.empty())
			{
				throw std::invalid_argument("Invalid database parameters");
			}
			ConnectionsPool::login = params;
			isSetParams = true;
		}
		static LoginData dbParameters()
		{
			LoginData db_param(ConnectionsPool::login);
			db_param.user_password = "";
			return db_param;
		}

		static bool isConnect()
		{
			return isConnected;
		}
		
		static ConnectionsPool& instance()
		{
			if (!isSetParams)
			{
				throw std::invalid_argument(
					"Connection parameters were not set. Call \"setParams\" "
					"before first call \"instance\".");
			}
			static ConnectionsPool instance;
			isConnected = true;
			return instance;
		}

		const ConnectionPointer connect()
		{
			std::unique_lock<std::mutex> lock_(mtx);
			while (pool.empty())
			{
				cond.wait(lock_);
			}
			auto conn = pool.front();
			pool.pop();
			return conn;
		}

		const void freeConnect(ConnectionPointer& connection)
		{
			std::unique_lock<std::mutex> lock_(mtx);
			pool.push(connection);
			lock_.unlock();
			cond.notify_one();
		}

		/**
		 * @name Prepared statements
		 *
		 * PostgreSQL supports prepared SQL statements, i.e. statements that can be
		 * registered under a client-provided name, optimized once by the backend, and
		 * executed any number of times under the given name.
		 */
		const void prepareQueries(Queries& queries)
		{
			std::vector<ConnectionPointer> connects;

			for (int i = 0; i < ConnectionsPool::login.num_connection; ++i)
			{
				ConnectionPointer conn = connect();
				connects.push_back(conn);
				for (const auto& elem : queries)
				{
					try
					{
						conn->prepare(elem.first, elem.second);
					}
					catch (std::exception& ex)
					{
						// Error preparing requests. The request may have already been prepared.
						LOG(ERROR) << "Error while preparing requests: [" << elem.first
							<< "] = " << elem.second << std::endl;
					}
				}
			}
			// free all connections
			if (connects.size() > 0)
			{
				for (size_t i = 0; i < connects.size(); ++i)
				{
					freeConnect(connects[i]);
				}
			}
		}

	private:
		ConnectionsPool()
		{

			std::string connection_string =
				"host=" + ConnectionsPool::login.host +
				" port=" + std::to_string(ConnectionsPool::login.port) +
				" user=" + ConnectionsPool::login.user_name +
				" password=" + ConnectionsPool::login.user_password +
				" dbname=" + ConnectionsPool::login.db_name;

			std::lock_guard<std::mutex> locker_(mtx);
			for (int i = 1; i <= ConnectionsPool::login.num_connection; ++i)
			{
				ConnectionPointer conn;
				try
				{
					conn.reset(new pqxx::connection(connection_string));
					pool.push(conn);
				}
				catch (std::exception& ex)
				{
					// Nothing to do. Other connections may work.
					LOG(ERROR) << "Connection exception: " << ex.what() << std::endl;
				}
			}

			int l_num_conn_real = pool.size();
			if (l_num_conn_real == 0)
			{
				throw std::invalid_argument("Couldn't connect to " + connection_string);
			}

			if (l_num_conn_real < ConnectionsPool::login.num_connection)
			{
				ConnectionsPool::login.num_connection = l_num_conn_real;
			}
		}

		ConnectionsPool(const ConnectionsPool& conn_pool) = delete;
		static LoginData login;

		static bool isSetParams;
		static bool isConnected;

		std::queue<ConnectionPointer> pool;
		std::mutex mtx;
		std::condition_variable cond;
	};

} // namespace dbpg

#endif