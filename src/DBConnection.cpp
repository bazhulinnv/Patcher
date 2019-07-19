#include "../include/DBProvider/DBConnection.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <exception>

namespace ParsingTools
{
	vector<string> splitToVector(string str, const string &delimiter)
	{
		vector<string> parsed;
		string token;

		size_t position = 0;
		while ((position = str.find(delimiter)) != string::npos)
		{
			token = str.substr(0, position);
			parsed.push_back(token);
			str.erase(0, position + delimiter.length());
		}

		return parsed;
	}

	string interpolate(string &original, const string &toBeReplaced, const string &replacement)
	{
		string newString = original.replace(original.find(toBeReplaced), toBeReplaced.length(), replacement);
		return newString;
	};

	string joinAsStrings(const vector<string> &vec, const char *delimiter)
	{
		stringstream res;
		copy(vec.begin(), vec.end(), ostream_iterator<string>(res, delimiter));
		return res.str();
	}


	pair<vector<string>, string> parseCredentials(string &input)
	{
		vector<string> params =
		{
			"dbname=${}",
			"user=${}",
			"password=${}"
			"hostname=${}",
			"port=${}",
			
		};

		vector<string> values = splitToVector(input, ":");
		vector<string> items;

		for (int i = 0; i < params.size(); ++i)
		{
			items.push_back(interpolate(params[i], "${}", values[i]));
		}

		const char *delim = " ";

		// Converts to a single string using stringstream
		string result = joinAsStrings(items, delim);
		return make_pair(values, result);
	}
}

DBConnection::DBConnection(std::string loginCredentials)
{
	// Parse string and get credentials
	std::pair<std::vector<std::string>, std::string> parsed = ParsingTools::parseCredentials(loginCredentials);

	// Save login info
	info->databaseName = parsed.first[0];
	info->username = parsed.first[1];
	info->password = parsed.first[2];
	info->hostname = parsed.first[3];
	info->portNumber = stoi(parsed.first[4]);
	info->result = parsed.second;

	setConnection(parsed.second);
}

DBConnection::~DBConnection()
{
	disconnect();
	delete info;
}

void DBConnection::setConnection(const std::string &credentials)
{
	try
	{
		current = new pqxx::connection(std::string());
	}
	catch (const std::exception &e)
	{
		std::cerr << "ERROR: Could not establish connection." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void DBConnection::disconnect()
{
	current->disconnect();
}