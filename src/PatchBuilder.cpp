#include <DBProvider/DBProvider.h>
#include <PatchBuilder/PatchBuilder.h>

#include <iostream>

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
	cout << "Dependency list created" << endl;
}
