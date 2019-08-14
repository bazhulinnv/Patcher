#include "Shared/ParsingTools.h"

#include <iterator>
#include <sstream>

using namespace std;

string ParsingTools::interpolate(string input, const string& replacement, const string toBeReplaced)
{
	string newString = input.replace(input.find(toBeReplaced), toBeReplaced.length(), replacement);
	return newString;
}

string ParsingTools::interpolateAll(const string& input, queue<string> replacements, const string toBeReplaced)
{
	if (input.empty())
	{
		return string();
	}

	string res = input;
	size_t start_pos = 0;

	while ((start_pos = res.find(toBeReplaced, start_pos)) != string::npos && !replacements.empty())
	{
		res.replace(start_pos, toBeReplaced.length(), replacements.front());
		start_pos += replacements.front().length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		replacements.pop();
	}

	return res;
}

string ParsingTools::interpolateAll(const string& input, vector<string> replacements, const string toBeReplaced)
{
	if (toBeReplaced.empty() || input.empty())
	{
		return string();
	}

	string res = input;
	size_t start_pos = 0;

	while ((start_pos = res.find(toBeReplaced, start_pos)) != string::npos && !replacements.empty())
	{
		res.replace(start_pos, toBeReplaced.length(), replacements.front());
		start_pos += replacements.front().length();
		replacements.erase(replacements.begin());
	}

	return res;
}

// string string("hello $name");
// bool is_ok = replace(string, "Somename", "$name");
bool ParsingTools::replace(string& input, const string& replacement, const string& toBeReplaced)
{
	size_t start_pos = input.find(toBeReplaced);

	if (start_pos == string::npos)
	{
		return false;
	}

	input.replace(start_pos, toBeReplaced.length(), replacement);
	return true;
}

void ParsingTools::replaceAll(string& input, const string& replacement, const string& toBeReplaced)
{
	if (toBeReplaced.empty())
	{
		return;
	}

	size_t start_pos = 0;
	while ((start_pos = input.find(toBeReplaced, start_pos)) != string::npos)
	{
		input.replace(start_pos, toBeReplaced.length(), replacement);
		start_pos += replacement.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

vector<string> ParsingTools::splitToVector(string input, const string& delimiter)
{
	// In case empty string - result is empty vector<string>
	vector<string> result;

	while (input.size())
	{
		size_t index = input.find(delimiter);

		if (index != string::npos)
		{
			result.push_back(input.substr(0, index));
			input = input.substr(index + delimiter.size());

			if (input.size() == 0)
			{
				result.push_back(input);
			}
		}
		else
		{
			result.push_back(input);
			input = "";
		}
	}

	return result;
}

string ParsingTools::joinAsString(const vector<string>& input, const char* delimiter)
{
	// If vector is empty, return empty string
	if (input.empty()) return string("");

	stringstream res;
	copy(input.begin(), input.end(), ostream_iterator<string>(res, delimiter));
	return res.str();
}

string ParsingTools::parseCredentials(const string& pgLogin)
{
	vector<string> params =
	{
		"hostaddr=${}",
		"port=${}",
		"dbname=${}",
		"user=${}",
		"password=${}",
	};

	vector<string> values = splitToVector(pgLogin, ":");
	if (values[0] == "localhost" || values[0] == "loopback") values[0] = "127.0.0.1";

	vector<string> items;
	for (int i = 0; i < params.size(); ++i)
	{
		items.push_back(interpolate(params[i], values[i]));
	}

	return joinAsString(items, " ");
}

pair<bool, string> ParsingTools::tryParseCredentials(const string& pgLogin)
{
	string result;
	vector<string> parsed;
	vector<string> params =
	{
		"hostaddr=${}",
		"port=${}",
		"dbname=${}",
		"user=${}",
		"password=${}",
	};

	try
	{
		vector<string> values = splitToVector(pgLogin, ":");
		if (values[0] == "localhost" || values[0] == "loopback") values[0] = "127.0.0.1";

		for (int i = 0; i < params.size(); ++i)
		{
			parsed.push_back(interpolate(params[i], values[i]));
		}
	}
	catch (const exception&)
	{
		return make_pair(false, joinAsString(parsed, " "));
	}

	// If parsing succeeded
	// convert to single string using stringstream
	return make_pair(true, joinAsString(parsed, " "));
}
