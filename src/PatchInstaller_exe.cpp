#include <iostream>
#include <fstream>
#include <filesystem>
#include <direct.h>
#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "PatchInstaller/FileParser.h"

bool directoryExists(char* directory) {
	const std::filesystem::path path(directory);
	return std::filesystem::exists(directory);
}

inline bool fileExists(char* directory) {
	chdir(directory);
	std::ifstream f("Install.bat");
	return f.good();
}

int main(int argc, char* argv[]) { 
	std::string nameOfFile =  "DependencyList.dpn";
	PatchInstaller patchInstaller;
	if (argv[1] == nullptr) {
		std::cerr << "Please, enter database connection parameters. \n";
		return -1;
	}
	else {
		std::string parameters(argv[1]);
		try {
			DBProvider *dbProvider = new DBProvider(parameters);
			if (argv[2] == nullptr || !(strcmp(argv[2], "check") == 0 || strcmp(argv[2], "install") == 0)) {
				std::cerr << "Wrong command of installer. Choose install/check. \n";
				return -1;
			}
			else {
				if (argv[3] == nullptr) {
					std::cerr << "Please, enter the directory of installation script.\n";
					return -1;
				}
				if (!directoryExists(argv[3])) {
					std::cerr << "Directory does not exists or wrong.\n";
					return -1;
				}
				else {
					if (fileExists(argv[3])) {
						mkdir("Temp");
						if (strcmp(argv[2], "check") == 0) {
							patchInstaller.checkObjectsForExistenceFromFile(nameOfFile, *dbProvider);
							return 0;
						}
						if (strcmp(argv[2], "install") == 0) {
							patchInstaller.startInstallation();
							return 0;
						}
					}
					else {
						std::cerr << "Directory does not exists or wrong.\n";
						return -1;
					}
				}
			}
		}
		catch (...) {
			std::cerr << "Please, enter right database connection parameters. \n";
			return -1;
		}
	}
}
