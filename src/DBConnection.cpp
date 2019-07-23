#include <DBProvider/DBConnection.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <exception>

namespace ParsingTools
{
	vector<string> splitToVector(string str, const string& delimiter)
	{
		vector<string> result;
		
		while (str.size())
		{
			size_t index = str.find(delimiter);
			
			if (index != string::npos)
			{
				result.push_back(str.substr(0, index));
				str = str.substr(index + delimiter.size());
				
				if (str.size() == 0)
				{
					result.push_back(str);
				}
			}
			else
			{
				result.push_back(str);
				str = "";
			}
		}

		return result;
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
			"password=${}",
			"hostaddr=${}",
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
	info.databaseName = parsed.first[0];
	info.username = parsed.first[1];
	info.password = parsed.first[2];
	info.host = parsed.first[3];
	info.portNumber = stoi(parsed.first[4]);

	info.result = parsed.second;
}

DBConnection::~DBConnection()
{
	disconnect();
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
	}
}

pqxx::connection* DBConnection::getConnection()
{
	return current;
}

void DBConnection::disconnect()
{
	current->disconnect();
}
