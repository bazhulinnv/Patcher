#include <iostream>
#include <fstream>
#include <filesystem>
#include <direct.h>
#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "PatchInstaller/FileParser.h"

bool directoryExists(char* directory) {
	return std::filesystem::exists(directory);
}

inline bool fileExists(char* directory) {
	std::string fileName = directory;
	fileName += "/Install.bat";
	return std::filesystem::exists(fileName);
}

void printNecessaryParameters() {
	std::cerr << "Connection parameters must be in this format: ";
	std::cerr << "name-of-database:user-name:user-password:host:port\n";
	std::cerr << "Example of right parameters: Database:User:password:127.0.0.1:5432\n";
}

int main(int argc, char* argv[]) { 
	PatchInstaller patchInstaller;
	bool returnCode = false;
	if (argv[1] == nullptr) {
		std::cerr << "There are no connection parameters to the database. Please, enter database connection parameters. \n";
		printNecessaryParameters();
		returnCode = true;
	}
	else {
		std::string parameters(argv[1]);
		try {
			auto *dbProvider = new DBProvider(parameters);
			if (argv[2] == nullptr || !(strcmp(argv[2], "check") == 0 || strcmp(argv[2], "install") == 0)) {
				std::cerr << "Wrong command of installer. Choose install/check. \n";
				returnCode = true;
			}
			else {
				if (argv[3] == nullptr) {
					std::cerr << "Please, enter the directory of installation script.\n";
					returnCode = true;
				}
				if (!directoryExists(argv[3])) {
					std::cerr << "Directory does not exists or wrong.\n";
					returnCode = true;
				}
				else {
					if (fileExists(argv[3])) {
						chdir(argv[3]);
						mkdir("Temp");
						if (strcmp(argv[2], "check") == 0) {
							patchInstaller.checkDependencyList("DependencyList.dpn", dbProvider);
						}
						if (strcmp(argv[2], "install") == 0) {
							patchInstaller.startInstallation();
						}
					}
					else {
						std::cerr << "There are no installation script in this directory. \n";
						returnCode = true;
					}
				}
			}

			delete dbProvider;
		}
		catch (...) {
			std::cerr << "Please, enter right database connection parameters. \n";
			printNecessaryParameters();
			returnCode = true;
		}

	}
	if (returnCode) {
		return -1;
	}
	return 0;
}
