#include <iostream>
#include <string>
#include <ctype.h>
#include "PatchBuilder/PatchBuilder.h"
#include "DBProvider/DBProvider.h"

using namespace std;

// Key values for ArgumentData
enum ArgumentKeys
{
	KEY_DIRECTORY, KEY_CONNECTION, KEY_PATH, KEY_TEMPLATE
};

// Structure for input arguments information
struct ArgumentData
{
	ArgumentData() : key(), is_required(false)
	{
	}

	ArgumentData(const string pDesciprion, const vector<string> pFlags, const ArgumentKeys pKey, const bool pIsRequired = true)
	{
		description = pDesciprion;
		flags = pFlags;
		key = pKey;
		is_required = pIsRequired;
	}
	string description; // description for help information
	vector<string> flags; // all flags for argument
	ArgumentKeys key; // unic key
	string value; // value of argument
	bool is_required; // is required to build patch
};

ArgumentData* getArgByFlag(vector<ArgumentData>& args, string flag) // Get argument from vector by flag
{
	transform(flag.begin(), flag.end(), flag.begin(), tolower); // Transform to lower register
	// Looking for a match with the flag
	for (ArgumentData& arg : args)
	{
		for (string& current_flag : arg.flags)
		{
			if (current_flag == flag)
			{
				return &arg; // Return the argument with the right flag
			}
		}
	}
	return nullptr;
}

ArgumentData* getArgumentByKey(vector<ArgumentData>& args, const ArgumentKeys &key) // Get argument from vector by key
{
	// Looking for a match with the лун
	for (ArgumentData& arg : args)
	{
		if (arg.key == key)
		{
			return &arg; // Return the argument with the right лун
		}
	}
	return nullptr;
}

vector<ArgumentData> getArguments() // Init all args data and return list of their
{
	vector<ArgumentData> args;
	args.push_back(ArgumentData("PatchList full path", { "-p", "-patch" }, KEY_PATH));
	args.push_back(ArgumentData("Patch building directory", { "-d", "-directory" }, KEY_DIRECTORY));
	args.push_back(ArgumentData("Connection arguments", { "-c", "-connection" }, KEY_CONNECTION));
	args.push_back(ArgumentData("Templates file full path [optional]", { "-t", "-template" }, KEY_TEMPLATE, false));
	return args;
}

void printHelp(const vector<ArgumentData>& params) // Print help information
{
	cout << "PatchBuilder - program for patch building by list of objects\n" << endl;
	cout << "To run the program with the necessary parameters, use the following flags:\n\n";
	for (ArgumentData param_data : params) // Print help information
	{
		cout << param_data.description << endl << "flags: ";
		for (const string flag : param_data.flags)
		{
			cout << flag << " ";
		}
		cout << endl << endl;
	}
	cout << "Examples:" << endl;
	cout << "1) -d C:\\PatchDirectory -p C:\\PatchList.txt   -c 127.0.0.1:5432:database:user:password   -t C:\\Templates.ini" << endl;
	cout << "2) -connect 127.0.0.1:5432:database:user:password   -patch C:\\PatchList.txt   -directory C:\\PatchDirectory" << endl;
}

int main(const int argc, char* argv[])
{
	vector<ArgumentData> args = getArguments(); // Get list of arguments

	for (int arg_index = 1; arg_index < argc; arg_index++)
	{
		// Showing help information in this case
		if ((argc == 2) && string(argv[arg_index]) == "-help")
		{
			printHelp(args);
			return 0;
		}
		else
		{
			ArgumentData* const param = getArgByFlag(args, argv[arg_index]); // Get argument by current flag
			if (param != nullptr && arg_index < argc - 1) // if exist argument with current flag and it is not las argument
			{
				arg_index++;
				param->value = string(argv[arg_index]);
			}
		}
	}

	// Checking on emptiness of required arguments values
	for (ArgumentData& param : args)
	{
		if (param.value.empty() && param.is_required)
		{
			cout << "You must specify the following argument:\n" << param.description;
			return -1;
		}
	}

	// If everything is fine with the arguments try to build path
	PatchBuilder *builder;
	try
	{
		DBProvider provider(getArgumentByKey(args, KEY_CONNECTION)->value);
		const string template_path = getArgumentByKey(args, KEY_TEMPLATE)->value;
		builder = new PatchBuilder(getArgumentByKey(args, KEY_PATH)->value, provider, template_path);
		builder->buildPatch(getArgumentByKey(args, KEY_DIRECTORY)->value);
		return 0;
	}
	catch (exception& err)
	{
		cerr << err.what() << endl;
		builder->addLog(err.what());
		return -1;
	}

	return 0;
}