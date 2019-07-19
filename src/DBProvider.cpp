#include <DBProvider/DBProvider.h>
#include <pqxx/pqxx>
#include <pqxx/transaction>
#include <iostream>

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

pqxx::result DBProvider::query(std::string stringSQL)
{	
	// Connection must be already set
	if (!conn->getConnection())
	{
		throw new std::exception("ERROR: Connection was not set.\n");
	}

	pqxx::work trans(*conn->getConnection(), "trans");

	// Get result from database
	pqxx::result res = trans.exec(stringSQL);
	trans.commit();
	return res;
}

void printObjectsData(pqxx::result queryResult)
{
	// Iterate over the rows in our result set.
	// Results objects are containers similar to std::vector and such.
	for (	pqxx::result::const_iterator row = queryResult.begin();
			row != queryResult.end();
			++row )
	{
		std::cout
			<< row["obj_schema"].as<std::string>() << "\t"
			<< row["obj_name"].as<std::string>() << "\t"
			<< row["obj_type"].as<std::string>()
			<< std::endl;
	}
}
