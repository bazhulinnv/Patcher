#include <iostream>
#include <fstream>
#include <filesystem>
#include <direct.h>
#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "PatchInstaller/FileParser.h"
#include "Shared/ParsingTools.h"

bool directoryExists(char* directory) {
	return std::filesystem::exists(directory);
}

inline bool fileExists(char* directory) {
	std::string fileName = directory;
	fileName += "/Install.bat";
	return std::filesystem::exists(fileName);
}

void printNecessaryParameters() {
	std::cerr << "Connection parameters must be in following format: ";
	std::cerr << "name-of-database:user-name:user-password:host:port\n";
	std::cerr << "Example of correct parameters: Database:User:password:127.0.0.1:5432\n";
}

int main(int argc, char* argv[]) { 
	PatchInstaller patchInstaller;
	bool returnCode = false;
	if (argv[1] == nullptr) {
		std::cerr << "Please, enter database connection parameters. \n";
		printNecessaryParameters();
		returnCode = true;
	}
	else {
		std::string parameters(argv[1]);
		try {
			auto *dbProvider = new DBProvider(parameters);
			std::pair<std::vector<std::string>, std::string> separateParameters = ParsingTools::parseCredentials(parameters);

			if (argv[2] == nullptr || !(strcmp(argv[2], "check") == 0 || strcmp(argv[2], "install") == 0)) {
				std::cerr << "Incorrect installer command. Choose install/check. \n";
				returnCode = true;
			}
			else {
				if (argv[3] == nullptr) {
					std::cerr << "Please, enter installation script directory.\n";
					returnCode = true;
				}
				if (!directoryExists(argv[3])) {
					std::cerr << "Directory does not exist or path has incorrect format.\n";
					returnCode = true;
				}
				else {
					if (fileExists(argv[3])) {
						chdir(argv[3]);
						mkdir("Temp");
						if (strcmp(argv[2], "check") == 0) {
							try {
								patchInstaller.checkDependencyList("DependencyList.dpn", dbProvider);
							}
							catch (std::invalid_argument exception) {
								std::cerr << "Invalid format of DependencyList.dpn\n";
							}
						}
						if (strcmp(argv[2], "install") == 0) {
							patchInstaller.startInstallation(separateParameters);
						}
					}
					else {
						std::cerr << "There is no installation script in this directory. \n";
						returnCode = true;
					}
				}
			}

			delete dbProvider;
		}
		catch (std::exception e) {
			std::cerr << "Please, enter correct database connection parameters. \n";
			printNecessaryParameters();
			returnCode = true;
		}

	}
	if (returnCode) {
		return -1;
	}
	return 0;
}
