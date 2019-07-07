#include "DBProvider/DBProvider.h"
#include "PatchBuilder/PatchBuilder.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

using namespace std;

PatchBuilder::PatchBuilder(string pPatchListDirectory)
{
	patchListDirectory = pPatchListDirectory;
}
PatchBuilder::~PatchBuilder() {}

void PatchBuilder::buildPatch(string directory)
{
	createInstallPocket(directory);
	createInstallScript(directory);
	createDependencyList(directory);
	cout << "Patch builded!" << endl;
}

void PatchBuilder::createInstallPocket(string directory)
{
	cout << "Install pocket created" << endl;
}

void PatchBuilder::createObjectList(string directory)
{
	cout << "Object list created" << endl;
}

void PatchBuilder::createInstallScript(string directory)
{
	cout << "Install script created" << endl;
}

void PatchBuilder::createDependencyList(string directory)
{
	vector<pair<string, string>> objectVector;
	string name;
	string type;
	pair<string, string> newPair;

	ifstream input(directory + objectListName);
	if (input.is_open())
	{
		while (!input.eof())
		{
			input >> name;
			input >> type;
			newPair.first = name;
			newPair.second = type;
			objectVector.push_back(newPair);
		}
		input.close();
	}

	ofstream output(directory + dependencyListName);
	if (output.is_open())
	{
		for (pair<string, string> objectPair : objectVector)
		{
			output << objectPair.first << " ";
			output << objectPair.second << endl;
		}
	}
	output.close();

	cout << "Dependency list created" << endl;
}

bool PatchBuilder::isContains(string objectName, string objectType, string scriptFullName)
{
	return false;
}
