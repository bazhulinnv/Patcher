#include <iostream>
#include <fstream>
#include <streambuf>
#include <direct.h>
#include <regex>
#include "DBProvider/DBProvider.h"
#include "PatchBuilder/PatchBuilder.h"

using namespace std;

PatchBuilder::PatchBuilder(const string pPatchListFullName)
{
	// Initialisation of class fields
	patchListFullName = pPatchListFullName;
}
PatchBuilder::~PatchBuilder() {}

void PatchBuilder::buildPatch(const string directory)
{
	// Executing all methods for patch building
	ofstream output(directory + "\\" + dependencyListName); // Dependency list directory
	scriptDataVectorType scriptDataVector = getScriptDataVector(); // Getting all scripts created by DBProvider
	creatInstallPocket(directory, scriptDataVector); // Creaing all instalation components
	removeCommits(scriptDataVector);
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
	ifstream input("C:\\Users\\Timur\\Documents\\Temp\\ObjectList.txt");
	if (input.is_open())
	{
		while (!input.eof())
		{
			ObjectData data;
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
	// Creating sql files for all scrpits
	for (ScriptData data : scriptDataVector)
	{
		// Creating directory named as type of script
		mkdir(&(directory + "\\" + data.type)[0]);
		ofstream output(directory + "\\" + data.type + "\\" + data.name);
		// Writing script text in file
		output << data.text;
	}
	// Creating of install script not added yet
	cout << "Install pocket created" << endl;
}

bool PatchBuilder::isContains(const ObjectData data, const string &scriptText) const
{
	// Checking on the content of the object in current script
	cmatch res; // To contain result of searching
	string str = data.name;
	regex rx(str); // Creating regular expression 
	if (regex_search(scriptText.c_str(), res, rx))
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
		ifstream input("C:\\Users\\Timur\\Documents\\public\\tables\\roles.sql");
		string str((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "users.sql";
		data.type = "table";
		ifstream input("C:\\Users\\Timur\\Documents\\public\\tables\\users.sql");
		string str((std::istreambuf_iterator<char>(input)),
			std::istreambuf_iterator<char>());
		str.erase(0, 3); //Utf8 mark delition
		data.text = str;
		scriptDataVector.push_back(data);
	}
	{
		data.name = "placeholder.sql";
		data.type = "table";
		ifstream input("C:\\Users\\Timur\\Documents\\public\\tables\\placeholder.sql");
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

void PatchBuilder::removeCommits(scriptDataVectorType & scriptDataVector)
{
	// Removing of all commits
	for (ScriptData &data : scriptDataVector)
	{
		// "--" comits removing
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

		// "/* */" commits removing
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
