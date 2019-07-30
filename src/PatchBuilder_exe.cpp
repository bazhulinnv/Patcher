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


struct ArgumentData // Structure for input arguments information
{
	ArgumentData(): key(), isRequired(false)
	{
	}

	ArgumentData(const string pDesciprion, const vector<string> pFlags, const ArgumentKeys pKey, const bool pIsRequired = true) 
	{
		description = pDesciprion;
		flags = pFlags;
		key = pKey;
		isRequired = pIsRequired;
	}
	string description; // description for help information
	vector<string> flags; // all flags for argument
	ArgumentKeys key; // unic key
	string value; // value of argument
	bool isRequired; // is required to build patch
};

ArgumentData * getArgByFlag(vector<ArgumentData> &args, string flag) // Get argument from vector by flag
{
	transform(flag.begin(), flag.end(), flag.begin(), tolower); // Transform to lower register
	// Looking for a match with the flag
	for (ArgumentData &arg : args)
	{
		for (string &currentFlag : arg.flags)
		{
			if (currentFlag == flag)
			{
				return &arg; // Return the argument with the right flag
			}
		}
	}
	return nullptr;
}

ArgumentData * getArgumentByKey(vector<ArgumentData> &args, ArgumentKeys key) // Get argument from vector by key
{
	// Looking for a match with the лун
	for (ArgumentData &arg : args)
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
	args.push_back(ArgumentData("Connection arguments", { "-c", "-connection"}, KEY_CONNECTION));
	args.push_back(ArgumentData("Templates file full path", { "-t", "-template" }, KEY_TEMPLATE, false));
	return args;
}

void printHelp(const vector<ArgumentData> &params) // Print help information
{
	cout << "PatchBuilder - program for patch building by list of objects\n" << endl;
	cout << "To run the program with the necessary parameters, use the following flags:\n\n";
	for (ArgumentData paramData : params) // Print help information
	{
		cout << paramData.description << endl << "flags: ";
		for (const string flag : paramData.flags)
		{
			cout << flag << " ";
		}
		cout << endl << endl;
	}
}

int main(const int argc, char *argv[])
{
	vector<ArgumentData> args = getArguments(); // Get list of arguments

	for (int argIndex = 1; argIndex < argc; argIndex++)
	{
		// Showing help information in this case
		if ((argc == 2) && string(argv[argIndex]) == "-help")
		{
			printHelp(args);
			return 1;
		}
		else
		{
			ArgumentData *const param = getArgByFlag(args, argv[argIndex]); // Get argument by current flag
			if (param != nullptr && argIndex < argc - 1) // if exist argument with current flag and it is not las argument
			{
				argIndex++;
				param->value = string(argv[argIndex]);
			}
		}
	}

	// Checking on emptiness of required arguments values
	for (ArgumentData &param : args)
	{
		if (param.value.empty() && param.isRequired )
		{
			cout << "You must specify the following argument:\n" << param.description;
			return -1;
		}
	}

	// If everything is fine with the arguments try to build path
	try
	{
		DBProvider provider(getArgumentByKey(args, KEY_CONNECTION)->value);
		cout << (provider.getScriptData(ObjectData("placeholder", "table", "public"))).text;
		//string templatePath = getArgumentByKey(args, KEY_TEMPLATE)->value;
		//PatchBuilder builder(getArgumentByKey(args, KEY_PATH)->value, provider, templatePath);
		//builder.buildPatch(getArgumentByKey(args, KEY_DIRECTORY)->value);
		return 1;
	}
	catch (exception &err)
	{
		cerr << err.what() << endl;
		return -1;
	}

	return 1;
}