#include <cstdio>
#include <PatchInstaller/PatchInstaller.h>
#include "DBProvider/DBProvider.h"
#include <String>

using namespace std;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}

bool PatchInstaller::checkObjectsForExistence(FILE* file)
{
	bool result = true;
	DBProvider dbProvider;
	//Generate file, when for each object write exist it, or not
	FILE* fileWithObjectsExistence;
	while (!feof(file)) {
		//parsing of file with list of objects
		result = result && dbProvider.isCurrentObjectExist();
		if (!dbProvider.isCurrentObjectExist()) {
			//writing to logging file with existence of objects
		}
	}
	fclose(fileWithObjectsExistence);
	return result;
}

bool startInstallation(FILE* fileWithDependencies, std::string objectsAndScriptsDirectory, std::string installationScriptDirectory) {
	PatchInstaller patchInstaller;
	//Take the list of dependencies and check it for the presence in database
	if (patchInstaller.checkObjectsForExistence(fileWithDependencies)) {
		//Check succeed
		//Start installation script
		FILE* installationSctipt;
		//Generate logging file with installation mistakes
	}
	else {
		//Check failed (some object does not exist)
		return false;
	}
}