#include <DBProvider/DBProvider.h>
#include <pqxx/pqxx>
#include <pqxx/transaction>

DBProvider::DBProvider(std::string args)
{
	conn = new DBConnection(args);
	conn->setConnection();
}

DBProvider::~DBProvider()
{
	delete conn;
}

std::vector<std::tuple<Schema, ObjectName, ObjectType>> DBProvider::getObjects()
{
	return std::vector<std::tuple<Schema, ObjectName, ObjectType>>();
}

pqxx::result DBProvider::query(std::string strSQL)
{	
	// Connection must be already set
	if (!conn->getConnection())
	{
		throw new std::exception("ERROR: Connection was not set.\n");
	}

	pqxx::work trans(*conn->getConnection(), "trans");

	// Get result from database
	pqxx::result res = trans.exec(strSQL);
	trans.commit();
	return res;
}