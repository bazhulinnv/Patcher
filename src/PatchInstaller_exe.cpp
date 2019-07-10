#include <iostream>
#include <fstream>

#include "PatchInstaller/PatchInstaller.h"

using namespace std;

int main() { 
	std::string nameOfFile =  "dependencies.txt";
	PatchInstaller patchInstaller;
	patchInstaller.checkObjectsForExistence(nameOfFile);
	patchInstaller.startInstallation();
	return 0;
}
