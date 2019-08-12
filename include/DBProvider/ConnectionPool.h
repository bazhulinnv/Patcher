//#ifndef CONNECTIONPOOL_H
//#define CONNECTIONPOOL_H
//
//#include <mutex>
//#include <pqxx/pqxx>
//#include <memory>
//#include <queue>
//#include <iostream>
//#include "Shared/ParsingTools.h"
//
//#include <string>
//
//namespace DBConnect
//{
//	class DBConnectionPool
//	{
//	};
//
//
//
//	static DBConnectionPool& instance()
//	{
//		if (!isSetParams)
//		{
//			throw std::invalid_argument(
//				"Connection parameters were not set. Call \"setParams\" "
//				"before first call \"instance\".");
//		}
//		static DBConnectionPool instance;
//		isConnected = true;
//		return instance;
//	}
//
//	class Inner
//	{
//	public:
//		Inner()
//		{
//			connection = instance().connect();
//		}
//
//		~Inner()
//		{
//			instance().freeConnect(connection);
//		}
//
//		pqxx::connection& operator*()
//		{
//			return *connection;
//		}
//
//	private:
//		std::shared_ptr<pqxx::connection> connection;
//	};
//
//
//}
//
//#endif