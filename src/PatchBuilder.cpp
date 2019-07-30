#include <iostream>
#include <fstream>
#include <streambuf>
#include <direct.h>
#include <ctime>
#include <cstdio>
#include <exception>
#include "PatchBuilder/PatchBuilder.h"

using namespace std;

PatchBuilder::PatchBuilder(const string pPatchListFullName, DBProvider &pProvider, const string pTemplatesFullName)
{
	patchListFullName = pPatchListFullName;
	provider = &pProvider;
	// Check tamplate file
	ifstream input;
	input.open(pTemplatesFullName);
	if (input.is_open())
	{
		templateString = string((istreambuf_iterator<char>(input)), istreambuf_iterator<char>()); // Reading all text file in string
	}
	else
	{
		const string message = "Cannot open or find template file\nDependencyList will be formed by presence of names\n";
		isWithWarnings = true;
		cerr << message;
		addLog(message);
	}
}
PatchBuilder::~PatchBuilder() {}

void PatchBuilder::buildPatch(const string directory)
{
	// Creating log file
	string logDirectory = (directory + "/" + LOG_FOLDER);
	_mkdir(&logDirectory[0]);
	logFileFullName = logDirectory + "/" + LOG_NAME + getCurrentDateTime() + LOG_FORMAT;

	// Executing all methods for patch building
	ofstream output(directory + "/" + DEPENDENCY_LIST_NAME); // Dependency list directory
	const objectDataVectorType patchListVector = getPatchListVector(); // Getting vector that contains all patch objects
	scriptDataVectorType scriptDataVector = getScriptDataVector(patchListVector); // Getting all scripts created by DBProvider
	createInstallPocket(directory, scriptDataVector); // Creaing all instalation components
	removeComments(scriptDataVector);
	objectDataVectorType objectDataVector = getObjectDataVector(); // Getting vector that contains all objects of source databse
	remove(objectDataVector, patchListVector); // Removing path objects from objectDataVector

	// Writing of DependencyList
	string message = string("Parsing started...\n")  +  BLOCK_LINE + "\n";
	cout << message;
	addLog(message);
	for (const ObjectData objectData : objectDataVector)
	{
		// Checking all objects for the presence in scripts
		for (const ScriptData scriptData : scriptDataVector)
		{
			if (isContains(objectData, scriptData.text))
			{
				// If object was found - writing it's name and type in DependencyList
				output << objectData.scheme << " ";
				output << objectData.name << " ";
				output << objectData.type << endl;
				message = " - " + objectData.name + " with " + objectData.type + " type included\n";
				cout << message;
				addLog(message);
				break;
			}
		}
	}
	output.close();
	cout << BLOCK_LINE << endl;
	addLog(BLOCK_LINE + string("\n"));

	if (isWithErrors)
	{
		message = "Patch built with errors!\n";\
	}
	else
	{
		if (isWithWarnings)
		{
			message = "Patch built with warnings!\n";
		}
		else
		{
			message = "Patch built successfully!\n";
		}
	}
	cout << message;
	addLog(message);
}

scriptDataVectorType PatchBuilder::getScriptDataVector(const objectDataVectorType &objectDataVector) const
/*const*/
{
	// Not implemented
	scriptDataVectorType scriptDataVector;
	fillScriptDataVector(scriptDataVector); // Temp
	// Add in vector outside scripts from patchListVector
	for (ObjectData objectData : objectDataVector)
	{
		if (objectData.scheme == "script")
		{
			// Reading all text from file
			ifstream input(objectData.name);
			const string text((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());

			// Remove path to file leave only name
			const size_t slashPos = objectData.name.find_last_of("\\");
			objectData.name.erase(0, slashPos+1);

			// Add script in vector
			const ScriptData scriptData(objectData, text);
			scriptDataVector.push_back(scriptData);
		}
	}
	const string message =  "Script vector created\n";
	cout << message;
	addLog(message);
	return scriptDataVector;
}

objectDataVectorType PatchBuilder::getObjectDataVector() const
{
	// Getting all source database objects
	objectDataVectorType objectVector = provider->getObjects();

	const string message = "Object vector created\n";
	cout << message;
	addLog(message);

	return objectVector;
}

void PatchBuilder::createInstallPocket(const string directory, const scriptDataVectorType &scriptDataVector) const
{
	ofstream outputInstallScriptBat(directory + "/" + INSTALL_SCRIPT_NAME_BAT);
	ofstream outputInstallScriptSh(directory + "/" + INSTALL_SCRIPT_NAME_SH);
	// Creating sql files for all scrpits and writing install script
	for (const ScriptData data : scriptDataVector)
	{
		// Creating directory named as type of script
		_mkdir(&(directory + "/" + data.scheme)[0]);
		_mkdir(&(directory + "/" + data.scheme + "/" + data.type)[0]);
		ofstream outputScript(directory + "/" + data.scheme + "/" + data.type + "/" + data.name);
		// Writing script text in file
		outputScript << data.text;

		// Creating install command
		string installBatStr = string("psql -U ") + "%1" + " -d " + "%2" + " -h " + "%3" + " -p " + "%4" " -f " + data.scheme + "/";
		string installShStr = string("psql -U ") + "$1" + " -d " + "$2" + " -h " + "$3" + " -p " + "$4" " -f " + data.scheme + "/";
		if (data.type != "")
		{
			installBatStr += data.type + "/";
			installShStr += data.type + "/";
		}
		installBatStr += data.name + "\n";
		installShStr += data.name + "\n";


		outputInstallScriptBat << installBatStr; // Writing psql command in InstallScript with .bat format
		outputInstallScriptSh << installShStr; // Writing psql command in InstallScript with .sh format	
	}
	const string message = "Install pocket created\n";
	cout << message;
	addLog(message);
}

bool PatchBuilder::isContains(const ObjectData data, const string &scriptText)
{
	cmatch result; // To contain result of searching

	// If template is empty - searching by object name
	if (templateString == "")
	{
		return (regex_search(scriptText.c_str(), result, regex(data.name)));
	}

	// Checking on the content of the object in current script
	const regex regularExpression = createExpression(data); // Creating regular expression 
	try
	{
		return (regex_search(scriptText.c_str(), result, regularExpression));
	}
	catch (exception &err)
	{
		// If can not searching regular expression
		string waringStr = "WARNING - can not search regular expression from template:\nDESCRIPTION - ";
		waringStr += err.what();
		waringStr += "\n";
		waringStr += "returning false result for " + data.name + "\n";
		cerr << waringStr;
		addLog(waringStr);
		isWithWarnings = true;
		return false;
	}
}

objectDataVectorType PatchBuilder::getPatchListVector() const
{
	// Getting all patch objects
	objectDataVectorType patchListVector;
	ifstream input(patchListFullName);
	if (input.is_open())
	{
		while (!input.eof())
		{
			// Reading from PatchList file in patchListVector
			ObjectData data;
			input >> data.scheme;
			// If this is script from outside - type field is empty
			if (data.scheme == "script")
			{
				input >> data.name;
				data.type = "";
			}
			else
			{
				input >> data.name;
				input >> data.type;
			}
			// If type is "function" reading params of it
			if (data.type == "function")
			{
				string currentWord;
				input >> currentWord;
				input >> currentWord;
				while (currentWord != ")")
				{
					data.paramsVector.push_back(currentWord);
					input >> currentWord;
				}
			}
			patchListVector.push_back(data);
		}
	}

	return patchListVector;
}

void PatchBuilder::fillScriptDataVector(scriptDataVectorType &scriptDataVector)
{
	// Temp
	ScriptData data;
	{
		data.name = "roles.sql";
		data.type = "table";
		data.scheme = "public";
		ifstream input("G:/Timur/ProjectFiles/Doors/public/tables/roles.sql");
		string str((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "users.sql";
		data.type = "table";
		data.scheme = "public";
		ifstream input("G:/Timur/ProjectFiles/Doors/public/tables/roles.sql");
		string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "placeholder.sql";
		data.type = "table";
		data.scheme = "public";
		ifstream input("G:/Timur/ProjectFiles/Doors/public/tables/placeholder.sql");
		const string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "user_full_info.sql";
		data.type = "view";
		data.scheme = "public";
		ifstream input("G:/Timur/ProjectFiles/Doors/public/views/user_full_info.sql");
		const string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "init_test.sql";
		data.type = "functions";
		data.scheme = "common";
		ifstream input("G:/Timur/ProjectFiles/Doors/common/functions/init_test.sql");
		const string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		data.text = str;
		scriptDataVector.push_back(data);
	}
}

void PatchBuilder::remove(objectDataVectorType &objectDataVector_first, const objectDataVectorType &objectDataVector_second)
{
	// Removing elements of second vector from first vector
	for (size_t index = 0; index < objectDataVector_first.size(); index++)
	{
		for (const ObjectData objectData : objectDataVector_second)
		{
			if (objectData == objectDataVector_first[index])
			{
				objectDataVector_first.erase(objectDataVector_first.begin() + index);
				index--;
				break;
			}
		}
	}
}

void PatchBuilder::removeComments(scriptDataVectorType &scriptDataVector)
{
	// Removing of all commits
	for (ScriptData &data : scriptDataVector)
	{
		// "--" comments removing
		string &text = data.text;
		size_t startPosition = text.find("--"); // Find "--" in text
		size_t endPosition;
		while (startPosition != string::npos)
		{
			// While can not to find "--" in text
			endPosition = text.find("\n", startPosition); // Find "new line symbol" in text after position of "--"
			text.erase(startPosition, endPosition - startPosition + 1); // Remove this part of text
			startPosition = text.find("--"); // Try to find next "--"
		}

		// "/* */" comments removing
		startPosition = text.find("/*"); // Find "/*" in text
		while (startPosition != string::npos)
		{
			// While can not to find "/*" in text
			endPosition = text.find("*/", startPosition); // Find "*/" in text after position of "/*"
			text.erase(startPosition, endPosition - startPosition + 2); // Remove this part of text
			startPosition = text.find("/*"); // Try to find next "/*"
		}
	}
}

regex PatchBuilder::createExpression(const ObjectData &data)
{
	// Determine the type of the object and use the appropriate template
	string currentWord = "";
	string regExStr = "";
	stringstream templateStream;
	templateStream << templateString;
	while (!templateStream.eof())
	{
		templateStream >> currentWord;
		// if have found type code word 
		if (currentWord == TYPE_CODE)
		{
			templateStream >> currentWord;
			templateStream >> currentWord;
			// Compare current template type with object type
			if (currentWord == data.type || currentWord == ANY_TYPE_CODE)
			{
				// If types are equal or it template for any type
				templateStream >> currentWord;
				templateStream >> currentWord;
				// Reading all reglular expressions
				// until the end code word is found
				while (currentWord != END_CODE)
				{
					// Replace name code word on current object name
					const size_t namePos = currentWord.find(NAME_CODE);
					if (namePos != string::npos)
					{
						currentWord.replace(namePos, NAME_LENGTH, data.name);
					}

					// Replace scheme code word on current object schema
					const size_t schemaPos = currentWord.find(SCHEME_CODE);
					if (schemaPos != string::npos)
					{
						currentWord.replace(schemaPos, SCHEME_LENGTH, data.scheme);
					}

					// Replace scheme code word on current object schema
					const size_t endlPos = currentWord.find("\n");
					if (endlPos != string::npos)
					{
						currentWord.pop_back();
						currentWord.pop_back();
					}

					// Concatenate this regular expressions with other
					regExStr += currentWord;
					regExStr += "|";

					// Reading next 
					templateStream >> currentWord;
				}

				// Remove last "|" symbol
				regExStr.pop_back();
				try 
				{
					return regex(regExStr);
				}
				catch (exception &err)
				{
					// If cannot create regular expression
					string warningStr = "WARNING - can not create regular expression from template:\nDESCRIPTION - ";
					warningStr += err.what();
					warningStr += "\n";
					warningStr += "returning simple expression by object name for " + data.name + "\n";
					cerr << warningStr;
					addLog(warningStr);
					isWithWarnings = true;
					return  regex(data.name);
				}
			}
		}
	}
	return {};
}

string PatchBuilder::getCurrentDateTime()
{
	// Getting current date
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%F-%H-%M-%S", &tstruct); // Returning time in "year-month-day-hour-minute-second" format
	return string(buf);
}

void PatchBuilder::addLog(const string message) const
{
	// Writing message in log file
	ofstream output(logFileFullName, std::ios_base::app);
	output << message;
	output.close();
}
