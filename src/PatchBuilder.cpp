#include <iostream>
#include <fstream>
#include <streambuf>
#include <direct.h>
#include <ctime>
#include <cstdio>
#include <exception>
#include "DBProvider/DBProvider.h"
#include "PatchBuilder/PatchBuilder.h"

using namespace std;

PatchBuilder::PatchBuilder(const string pPatchListFullName, const string pTemplatesFullName, const string pUserName, const string pDatabaseName)
{
	// Initialisation of class fields
	patchListFullName = pPatchListFullName;
	templatesFullName = pTemplatesFullName;
	userName = pUserName;
	databaseName = pDatabaseName;
}
PatchBuilder::~PatchBuilder() {}

void PatchBuilder::buildPatch(const string directory)
{
	// Creating log file
	string logDirectory = (directory + "//" + LOG_FOLDER);
	mkdir(&logDirectory[0]);
	logFileFullName = logDirectory + "//" + LOG_NAME + getCurrentDateTime() + LOG_FORMAT;

	// Executing all methods for patch building
	ofstream output(directory + "//" + DEPENDENCY_LIST_NAME); // Dependency list directory
	scriptDataVectorType scriptDataVector = getScriptDataVector(); // Getting all scripts created by DBProvider
	creatInstallPocket(directory, scriptDataVector); // Creaing all instalation components
	removeComments(scriptDataVector);
	objectDataVectorType objectDataVector = getObjectDataVector(); // Getting vector that contains all objects of source databse
	objectDataVectorType patchListVector = getPatchListVector(); // Getting vector that contains all patch objects
	remove(objectDataVector, patchListVector); // Removing path objects from objectDataVector

	// Writing of DependencyList
	cout << "Parsing started..." << endl << BLOCK_LINE << endl;
	for (ObjectData objectData : objectDataVector)
	{
		// Ñhecking all objects for the presence in scripts
		for (ScriptData scriptData : scriptDataVector)
		{
			if (isContains(objectData, scriptData.text))
			{
				// If object was found - writing it's name and type in DependencyList
				output << objectData.scheme << " ";
				output << objectData.name << " ";
				output << objectData.type << endl;
				break;
			}
		}
	}
	output.close();
	cout << BLOCK_LINE << endl;

	if (isSuccessfully)
	{
		cout << "Patch builded successfully!" << endl;
		addLog("Patch builded without errors");
	}
	else
	{
		cout << "Patch builded with errors!" << endl;
	}
}

scriptDataVectorType PatchBuilder::getScriptDataVector() /*const*/
{
	// Not implemented
	scriptDataVectorType scriptDataVector;
	fillScriptDataVector(scriptDataVector); // Temp
	cout << "Script vector created" << endl;
	return scriptDataVector;
}

objectDataVectorType PatchBuilder::getObjectDataVector() const
{
	// Getting all source database objects
	// Not implementeds
	objectDataVectorType objectVector;
	ifstream input("C://Users//Timur//Documents//Temp//ObjectList.txt");
	if (input.is_open())
	{
		while (!input.eof())
		{
			ObjectData data;
			input >> data.scheme;
			input >> data.name;
			input >> data.type;
			objectVector.push_back(data);
		}
	}
	cout << "Object vector created" << endl;
	return objectVector;
}

void PatchBuilder::creatInstallPocket(const string directory, const scriptDataVectorType &scriptDataVector) const
{
	ofstream outputInstallScriptBat(directory + "//" + INSTALL_SCRIPT_NAME_BAT);
	ofstream outputInstallScriptSh(directory + "//" + INSTALL_SCRIPT_NAME_SH);
	// Creating sql files for all scrpits and writing install script
	for (ScriptData data : scriptDataVector)
	{
		// Creating directory named as type of script
		mkdir(&(directory + "//" + data.scheme)[0]);
		mkdir(&(directory + "//" + data.scheme + "//" + data.type)[0]);
		ofstream outputScript(directory + "//" + data.scheme + "//" + data.type + "//" + data.name);
		// Writing script text in file
		outputScript << data.text;

		// Writing psql command in InstallScript with .bat format
		outputInstallScriptBat << "psql -U " << userName << " -d " << databaseName << " -f "  << data.scheme << "//" << data.type << "//" << data.name << "\n";

		// Writing psql command in InstallScript with .sh format
		outputInstallScriptSh << "psql -U " << userName << " -d " << databaseName << " -f " << data.scheme << "//" << data.type << "//" << data.name << "\n";
	}
	// Creating of install script not added yet
	cout << "Install pocket created" << endl;
}

bool PatchBuilder::isContains(const ObjectData data, const string &scriptText)
{
	// Checking on the content of the object in current script
	cmatch result; // To contain result of searching
	regex regularExpression = createExpression(data); // Creating regular expression 
	if (regex_search(scriptText.c_str(), result, regularExpression))
	{
		// If current expresiion was found return true
		cout << " - " << data.name << " with " << data.type << " type included" << endl;
		return true;
	}	
	return false;
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
			input >> data.name;
			input >> data.type;
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
		ifstream input("C://Users//Timur//Documents//Doors//public//tables//roles.sql");
		string str((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "users.sql";
		data.type = "table";
		data.scheme = "public";
		ifstream input("C://Users//Timur//Documents//Doors//public//tables//users.sql");
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
		ifstream input("C://Users//Timur//Documents//Doors//public//tables//placeholder.sql");
		string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "user_full_info.sql";
		data.type = "view";
		data.scheme = "public";
		ifstream input("C://Users//Timur//Documents//Doors//public//views//user_full_info.sql");
		string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "init_test.sql";
		data.type = "functions";
		data.scheme = "common";
		ifstream input("C://Users//Timur//Documents//Doors//common//functions//init_test.sql");
		string str((std::istreambuf_iterator<char>(input)),
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
		for (ObjectData objectData : objectDataVector_second)
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

void PatchBuilder::removeComments(scriptDataVectorType & scriptDataVector)
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

regex PatchBuilder::createExpression(ObjectData data)
{
	// Determine the type of the object and use the appropriate template
	ifstream input(templatesFullName);
	string currentWord = "";
	string regExStr = "";
	if (input.is_open())
	{
		while (!input.eof())
		{
			input >> currentWord;
			// if have found type code word 
			if (currentWord == TYPE_CODE)
			{
				input >> currentWord;
				input >> currentWord;
				// Compare current template type with object type
				if (currentWord == data.type || currentWord == ANY_TYPE_CODE)
				{
					// If types are equal or it template for any type
					input >> currentWord;
					input >> currentWord;
					// Reading all reglular expressions
					// until the end code word is found
					while (currentWord != END_CODE)
					{
						// Replace name code word on current object name
						size_t namePos = currentWord.find(NAME_CODE);
						if (namePos != string::npos)
						{
							currentWord.replace(namePos, NAME_LENGTH, data.name);
						}

						// Replace scheme code word on current object schema
						size_t schemaPos = currentWord.find(SCHEME_CODE);
						if (schemaPos != string::npos)
						{
							currentWord.replace(schemaPos, SCHEME_LENGTH, data.scheme);
						}

						// Concatenate this regular expressions with other
						regExStr += currentWord;
						regExStr += "|";

						// Reading next 
						input >> currentWord;
					}

					// Remove last "|" symbol
					regExStr.pop_back();
					try 
					{
						return regex(regExStr);
					}
					catch (exception &err)
					{
						// If can not create regular expression
						string errorStr = "ERROR - can not create regular expression from template:\nDESCRIPTION - ";
						errorStr += err.what();
						errorStr += "\n";
						errorStr += "returning simple expression by object name for " + data.name + "\n";
						cerr << errorStr;
						addLog(errorStr);
						isSuccessfully = false;
						return  regex(data.name);
					}
				}
			}
		}
	}

	// If file doesn't open return simple expression
	cout << "ERROR - can not open template file:" << endl;
	cout << "returning simple expression by object name for " << data.name << endl;
	return regex(data.name);
}

string PatchBuilder::getCurrentDateTime()
{
	// Getting current date
	time_t now = time(0);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%F-%H-%M-%S", &tstruct); // Returning time in "year-month-day-hour-minute-second" format
	return string(buf);
}

void PatchBuilder::addLog(string message)
{
	// Writing message in log file
	ofstream output(logFileFullName, std::ios_base::app);
	output << message;
	output.close();
}
