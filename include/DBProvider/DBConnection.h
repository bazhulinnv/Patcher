#pragma once
#include <string>
#include <vector>
#include <utility>
#include <exception>
#include <iostream>
#include <pqxx/pqxx>

namespace ParsingTools
{
	using namespace std;
	vector<string> splitToVector(string str, string delimiter);
	string interpolate(string & original, const string & toBeReplaced, const string & replacement);
	pair<vector<string>, char*> parseCredentials(string & input);
}

class DBConnection
{
public:

	struct LoginData
	{
		std::string hostname;
		unsigned int portNumber;
		std::string databaseName;
		std::string username;
		std::string password;
		char *result = nullptr;
	} *info;

	explicit DBConnection(std::string &loginCredentials)
	{
		// Parse string and get credentials
		std::pair<std::vector<std::string>, char *> parsed = ParsingTools::parseCredentials(loginCredentials);

		// Save login info
		this->info->hostname = parsed.first[0];
		this->info->portNumber = stoi(parsed.first[1], nullptr);
		this->info->databaseName = parsed.first[2];
		this->info->username = parsed.first[3];
		this->info->password = parsed.first[4];
		this->info->result = parsed.second;

		this->setConnection(parsed.second);
	}

	~DBConnection()
	{
		this->disconnect();
		delete currentConnection;
		delete info;
	}

private:
	// field stores current connection
	pqxx::connection *currentConnection = nullptr;
	void setConnection(const char *credentials);
	void disconnect();
};