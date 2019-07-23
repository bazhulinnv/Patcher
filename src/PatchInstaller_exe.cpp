#include <iostream>
#include <fstream>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "PatchInstaller/FileParser.h"

using namespace std;

int main(int argc, char* argv[]) { 
	std::string nameOfFile =  "DependencyList.dpn";
	PatchInstaller patchInstaller;

	auto *dbProvider = new DBProvider(nameOfFile);
	
	// gui pass 0, if user want to check; pass 1, if user want to install
	if (strcmp(argv[2], "check") == 0)
	{
		//std::cout << "Start checking:\n";
		patchInstaller.checkObjectsForExistenceFromFile(nameOfFile, *dbProvider);
	}

	if (strcmp(argv[2], "install") == 0)
	{
		//std::cout << "Start installation:\n";
		patchInstaller.startInstallation();
	}
	return 0;
}
