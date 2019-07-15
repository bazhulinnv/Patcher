#include <iostream>
#include <fstream>

#include "PatchInstaller/PatchInstaller.h"

using namespace std;

int main(int argc, char* argv[]) { 
	std::string nameOfFile =  "Dependencies.txt";
	PatchInstaller patchInstaller;
	DBProvider dbProvider;
	patchInstaller.passParametersToDBProvider(argv[1], dbProvider);
	// gui pass 0, if user want to check; pass 1, if user want to install
	if (strcmp(argv[2], "check") == 0) {
		patchInstaller.checkObjectsForExistence(nameOfFile, dbProvider);
	}
	if (strcmp(argv[2], "install") == 0) {
		patchInstaller.startInstallation();
	}
	return 0;
}
