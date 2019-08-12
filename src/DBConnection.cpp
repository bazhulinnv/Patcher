#include "DBProvider/DBConnection.h"
#include"Shared/ParsingTools.h"

#include <iostream>
#include <sstream>
#include <iterator>
#include <exception>

DBConnection::DBConnection(std::string loginCredentials)
{
	// Parse string and get credentials
	std::pair<std::vector<std::string>, std::string> parsed = ParsingTools::parseCredentials(loginCredentials);

	// Save login info
	info.databaseName = parsed.first[0];
	info.username = parsed.first[1];
	info.password = parsed.first[2];
	info.host = parsed.first[3];
	info.portNumber = stoi(parsed.first[4]);

	info.result = parsed.second;
}

DBConnection::~DBConnection()
{
	current->disconnect();
	delete current;
}

void DBConnection::setConnection()
{
	try
	{
		current = new pqxx::connection(info.result);
	}
	catch (const std::exception &e)
	{
		std::cerr << "ERROR: Could not establish connection." << std::endl;
		std::cerr << e.what() << std::endl;
		throw e;
	}
}

pqxx::connection* DBConnection::getConnection()
{
	if (!current)
	{
		throw new std::exception("ERROR:Connection to databse lost.\n");
	}

	return current;
}

void DBConnection::disconnect()
{
	if (!current)
	{
		return;
	}

	// use pqxx disconnect method
	current->disconnect();
}
