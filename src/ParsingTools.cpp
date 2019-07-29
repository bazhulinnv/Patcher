#include "Shared/ParsingTools.h"

#include <iterator>
#include <sstream>

using namespace std;

string ParsingTools::interpolate(string original, const string& replacement, const string& toBeReplaced)
{
	string newString = original.replace(original.find(toBeReplaced), toBeReplaced.length(), replacement);
	return newString;
}

string ParsingTools::interpolateAll(const string& str, queue<string> replacements, const string& toBeReplaced)
{
	if (str.empty())
	{
		return string();
	}

	string res = str;
	size_t start_pos = 0;

	while ((start_pos = res.find(toBeReplaced, start_pos)) != string::npos && !replacements.empty())
	{
		res.replace(start_pos, toBeReplaced.length(), replacements.front());
		start_pos += replacements.front().length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		replacements.pop();
	}

	return res;
}

string ParsingTools::interpolateAll(const string& str, vector<string> replacements, const string& toBeReplaced)
{
	if (toBeReplaced.empty() || str.empty())
	{
		return string();
	}

	string res = str;
	size_t start_pos = 0;

	while ((start_pos = res.find(toBeReplaced, start_pos)) != string::npos && !replacements.empty())
	{
		res.replace(start_pos, toBeReplaced.length(), replacements.front());
		start_pos += replacements.front().length();
		replacements.erase(replacements.begin());
	}

	return res;
}

bool ParsingTools::replace(string& str, const string& from, const string& to)
{
	size_t start_pos = str.find(from);

	if (start_pos == string::npos)
	{
		return false;
	}

	str.replace(start_pos, from.length(), to);
	return true;
}

void ParsingTools::replaceAll(string& str, const string& from, const string& to)
{
	if (from.empty())
	{
		return;
	}

	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

vector<string> ParsingTools::splitToVector(string str, const string& delimiter)
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

string ParsingTools::joinAsStrings(const vector<string>& vec, const char* delimiter)
{
	stringstream res;
	copy(vec.begin(), vec.end(), ostream_iterator<string>(res, delimiter));
	return res.str();
}

pair<vector<string>, string> ParsingTools::parseCredentials(string& input)
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
		items.push_back(interpolate(params[i], values[i], "${}"));
	}

	const char* delim = " ";

	// Converts to a single string using stringstream
	string result = joinAsStrings(items, delim);
	return make_pair(values, result);
}
