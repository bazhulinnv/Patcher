#include <iostream>
#include <fstream>
#include <streambuf>
#include <direct.h>
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
	// Executing all methods for patch building
	ofstream output(directory + "//" + DEPENDENCY_LIST_NAME); // Dependency list directory
	scriptDataVectorType scriptDataVector = getScriptDataVector(); // Getting all scripts created by DBProvider
	creatInstallPocket(directory, scriptDataVector); // Creaing all instalation components
	removeComments(scriptDataVector);
	objectDataVectorType objectDataVector = getObjectDataVector(); // Getting vector that contains all objects of source databse
	objectDataVectorType patchListVector = getPatchListVector(); // Getting vector that contains all patch objects
	remove(objectDataVector, patchListVector); // Removing path objects from objectDataVector

	// Writing of DependencyList
	cout << "Parsing started" << endl;
	for (ObjectData objectData : objectDataVector)
	{
		// Ñhecking all objects for the presence in scripts
		for (ScriptData scriptData : scriptDataVector)
		{
			if (isContains(objectData, scriptData.text))
			{
				// If object was found - writing it's name and type in DependencyList
				output << objectData.name << " ";
				output << objectData.type << endl;
				break;
			}
		}
	}
	output.close();
	cout << "Patch builded successfully!" << endl;
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
			input >> data.schema;
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
		mkdir(&(directory + "//" + data.schema)[0]);
		mkdir(&(directory + "//" + data.schema + "//" + data.type)[0]);
		ofstream outputScript(directory + "//" + data.schema + "//" + data.type + "//" + data.name);
		// Writing script text in file
		outputScript << data.text;

		// Writing psql command in InstallScript with .bat format
		outputInstallScriptBat << "psql -U " << userName << " -d " << databaseName << " -f "  << data.schema << "//" << data.type << "//" << data.name << "\n";

		// Writing psql command in InstallScript with .sh format
		outputInstallScriptSh << "psql -U " << userName << " -d " << databaseName << " -f " << data.schema << "//" << data.type << "//" << data.name << "\n";
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
			input >> data.schema;
			input >> data.name;
			input >> data.type;
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
		data.schema = "public";
		ifstream input("C://Users//Timur//Documents//public//tables//roles.sql");
		string str((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "users.sql";
		data.type = "table";
		data.schema = "public";
		ifstream input("C://Users//Timur//Documents//public//tables//users.sql");
		string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "placeholder.sql";
		data.type = "table";
		data.schema = "public";
		ifstream input("C://Users//Timur//Documents//public//tables//placeholder.sql");
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
			if (currentWord == TYPE_CODE)
			{
				input >> currentWord;
				input >> currentWord;
				if (currentWord == data.type || currentWord == ANY_TYPE_CODE)
				{
					input >> currentWord;
					input >> currentWord;
					while (currentWord != END_CODE)
					{
						size_t namePos = currentWord.find(NAME_CODE);
						if (namePos != string::npos)
						{
							currentWord.replace(namePos, NAME_LENGTH, data.name);
						}
						size_t schemaPos = currentWord.find(SCHEMA_CODE);
						if (schemaPos != string::npos)
						{
							currentWord.replace(schemaPos, SCHEMA_LENGTH, data.schema);
						}
						size_t nextLinePos = currentWord.find("\n");
						regExStr += currentWord;
						regExStr += "|";
						input >> currentWord;
					}
					regExStr.pop_back();
					return regex(regExStr);
				}
			}
		}
	}
	return regex();
}
