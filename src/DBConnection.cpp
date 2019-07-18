#include "../include/DBProvider/DBConnection.h"

std::vector<std::string> ParsingTools::splitToVector(std::string str, std::string delimiter)
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

std::string ParsingTools::interpolate(std::string &original, const std::string &toBeReplaced, const std::string &replacement)
{
	string newString = original.replace(original.find(toBeReplaced), toBeReplaced.length(), replacement);
	return newString;
};

std::pair<std::vector<std::string>, char *> ParsingTools::parseCredentials(std::string &input)
{
	vector<string> params =
	{
		"username=${} ",
		"host=${} ",
		"password=${} ",
		"dbname=${}"
	};

	vector<string> values = splitToVector(input, ":");
	vector<string> items;

	for (int i = 0; i < params.size(); ++i)
	{
		items.push_back(interpolate(params[i], "${}", values[i]));
	}

	// Stores parsed input as charecter set
	char * output;

	// Convert all items to character set
	for (string item : items)
	{
		strcpy(output, item.c_str());
	}

	return make_pair(values, output);
}

void DBConnection::setConnection(const char * credentials)
{
	// Set connection
	try
	{
		this->currentConnection = new pqxx::connection(credentials);
	}
	catch (const std::exception &e)
	{
		std::cerr << "ERROR: Could not establish connection!" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void DBConnection::disconnect()
{
	this->currentConnection->disconnect();
}