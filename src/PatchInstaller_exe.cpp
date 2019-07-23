#include <iostream>
#include <fstream>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "PatchInstaller/FileParser.h"

int main(int argc, char* argv[]) { 
	std::string nameOfFile =  "DependencyList.dpn";
	PatchInstaller patchInstaller;
	// gui pass 0, if user want to check; pass 1, if user want to install
	std::string parameters(argv[1]);
	std::string installerDirectory(argv[3]);
	DBProvider *dbProvider = new DBProvider(parameters);
	patchInstaller.startInstallation(installerDirectory);
	if (strcmp(argv[2], "check") == 0)
	{
		//std::cout << "Start checking:\n";
		patchInstaller.checkObjectsForExistenceFromFile(nameOfFile, *dbProvider);
	}
	if (strcmp(argv[2], "install") == 0)
	{
		//std::cout << installerDirectory;
		std::cout << "WE ARE HERE!!!!";
		patchInstaller.startInstallation(installerDirectory);
	}
	delete dbProvider;
	return 0;
}
