//#ifndef CONNECTIONPOOL_H
//#define CONNECTIONPOOL_H
//
//#include "DBProvider/ConnectionThreadSafe.h"
//#include "DBProvider/LoginData.h"
//
//#include <mutex>
//#include <memory>
//#include <deque>
//
//namespace DBConnection
//{
//	class ConnectionPool
//	{
//	public:
//		ConnectionPool();
//		explicit ConnectionPool(int size);
//
//		void setConnections(LoginData params);
//
//		std::shared_ptr<ConnectionThreadSafe> connection();
//		void freeConnection(std::shared_ptr<ConnectionThreadSafe>);
//
//	private:
//		std::mutex mtx;
//		std::condition_variable poolCondition;
//		std::deque<std::shared_ptr<ConnectionThreadSafe>> connPool;
//		int poolSize = 10;
//
//		void createPool();
//	};
//};
//
//#endif