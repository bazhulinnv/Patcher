#include <iostream>
#include <string>
#include <ctype.h>
#include "PatchBuilder/PatchBuilder.h"
#include "DBProvider/DBProvider.h"

using namespace std;

// Key values for ArgData
enum ArgKey
{
	DIR_KEY, CON_KEY, PATCH_KEY, TEMPLATE_KEY
};


struct ArgData // Structure for input arguments information
{
	ArgData() {}
	ArgData(string pDesciprion, vector<string> pFlags, ArgKey pKey, bool pIsRequired = true) 
	{
		description = pDesciprion;
		flags = pFlags;
		key = pKey;
		isRequired = pIsRequired;
	}
	string description; // description for help information
	vector<string> flags; // all flags for argument
	ArgKey key; // unic key
	string value; // value of argument
	bool isRequired; // is required to build patch
};

ArgData *const getArgByFlag(vector<ArgData> &args, string flag) // Get argument from vector by flag
{
	transform(flag.begin(), flag.end(), flag.begin(), tolower);
	for (ArgData &arg : args)
	{
		for (string &currentFlag : arg.flags)
		{
			if (currentFlag == flag)
			{
				return &arg;
			}
		}
	}
	return nullptr;
}

ArgData *const getArgByKey(vector<ArgData> &args, ArgKey key) // Get argument from vector by key
{
	for (ArgData &arg : args)
	{
		if (arg.key == key)
		{
			return &arg;
		}
	}
	return nullptr;
}

vector<ArgData> getArgsList() // Init all args data and return list of their
{
	vector<ArgData> args;
	args.push_back(ArgData("PatchList full path", { "-p", "-patch" }, PATCH_KEY));
	args.push_back(ArgData("Patch building directory", { "-d", "-directory" }, DIR_KEY));
	args.push_back(ArgData("Connection arguments", { "-c", "-connection"}, CON_KEY));
	args.push_back(ArgData("Templates file full path", { "-t", "-template" }, TEMPLATE_KEY, false));
	return args;
}

void printHelp(const vector<ArgData> &params) // Print help information
{
	cout << "PatchBuilder - program for patch building by list of objects\n" << endl;
	cout << "To run the program with the necessary parameters, use the following flags:\n\n";
	for (ArgData paramData : params) // Print help information
	{
		cout << paramData.description << endl << "flags: ";
		for (string flag : paramData.flags)
		{
			cout << flag << " ";
		}
		cout << endl << endl;
	}
}

int main(int argc, char *argv[])
{
	vector<ArgData> args = getArgsList(); // Get list of arguments

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
			ArgData *const param = getArgByFlag(args, argv[argIndex]); // Get argument by current flag
			if (param != nullptr && argIndex < argc - 1) // if exist argument with current flag and it is not las argument
			{
				argIndex++;
				param->value = string(argv[argIndex]);
			}
		}
	}

	// Checking on emptiness of required arguments values
	for (ArgData &param : args)
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
		DBProvider provider(getArgByKey(args, CON_KEY)->value);
		string templatePath = getArgByKey(args, TEMPLATE_KEY)->value;
		PatchBuilder builder(getArgByKey(args, PATCH_KEY)->value, provider, templatePath);
		builder.buildPatch(getArgByKey(args, DIR_KEY)->value);
		return 1;
	}
	catch (exception &err)
	{
		cerr << err.what() << endl;
		return -1;
	}

	return 1;
}