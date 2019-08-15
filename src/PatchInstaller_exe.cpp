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
	std::cerr << "host:port:name-of-database:username:user-password\n";
	std::cerr << "Example of correct parameters: localhost:5432:database:user:password\n";
}

/** Implementation of work with user. Handling different cases of incorrect input data. */
int main(int argc, char* argv[]) { 
	PatchInstaller patchInstaller;
	bool returnCode = false;
	if (argv[1] == nullptr) {
		// If user did not enter connection parameters
		std::cerr << "Please, enter database connection parameters. \n";
		printNecessaryParameters();
		returnCode = true;
	}
	else {
		std::string parameters(argv[1]);
		try {
			auto *dbProvider = new DBProvider(parameters);
			LoginData p(parameters);
			//std::pair<std::vector<std::string>, std::string> separateParameters = LoginData(parameters);

			// If user entered wrong command, like "checl" or "instal" instead "check" or "install"
			if (argv[2] == nullptr || !(strcmp(argv[2], "check") == 0 || strcmp(argv[2], "install") == 0)) {
				std::cerr << "Incorrect installer command. Choose install/check. \n";
				returnCode = true;
			}
			else {
				if (argv[3] == nullptr) {
					//If user did not entered installation script directory
					std::cerr << "Please, enter installation script directory.\n";
					returnCode = true;
				}
				if (!directoryExists(argv[3])) {
					//If there is not such directory in this computer, or entered directory is incorrect, for example C:/\####
					std::cerr << "Directory does not exist or path has incorrect format.\n";
					returnCode = true;
				}
				else {
					if (fileExists(argv[3])) {
						chdir(argv[3]);
						//Create folder for logs
						mkdir("logs");
						if (strcmp(argv[2], "check") == 0) {
							try {
								//
								patchInstaller.checkDependencyList("DependencyList.dpn", dbProvider);
							}
							catch (std::invalid_argument exception) {
								//If there is an object with incorrect number of parameters in the file (2 parameters instead 3)
								std::cerr << "Invalid format of DependencyList.dpn\n";
							}
						}
						if (strcmp(argv[2], "install") == 0) {
							patchInstaller.startInstallation(p);
						}
					}
					else {
						//Directory exists, but there is no Install.bat in it
						std::cerr << "There is no installation script in this directory. \n";
						returnCode = true;
					}
				}
			}

			delete dbProvider;
		}
		catch (std::exception e) {
			//Incorrect connection parameters were entered - incorrect port, host etc.
			std::cerr << "Please, enter correct database connection parameters. \n";
			printNecessaryParameters();
			returnCode = true;
		}

	}
	if (returnCode) {
		//If incorrect input data were in work of program
		return -1;
	}
	// If program started with correct parameters
	return 0;
}
