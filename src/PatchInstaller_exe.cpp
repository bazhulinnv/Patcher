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
	//chdir needs to be changed on more suitable variant
	chdir(directory);
	ifstream f("Install.bat");
	return f.good();
}

int main(int argc, char* argv[]) { 
	std::string nameOfFile =  "DependencyList.dpn";
	PatchInstaller patchInstaller;
	if (argv[1] == nullptr) {
		std::cout << "Please, enter database connection parameters. \n";
		return -1;
	}
	else {
		std::string parameters(argv[1]);
		try {
			DBProvider *dbProvider = new DBProvider(parameters);
			if (argv[2] == nullptr || !(strcmp(argv[2], "check") == 0 || strcmp(argv[2], "install") == 0)) {
				std::cout << "Wrong command of installer. Choose install/check. \n";
				return -1;
			}
			else {
				if (strcmp(argv[2], "check") == 0) {
					patchInstaller.checkObjectsForExistenceFromFile(nameOfFile, *dbProvider);
					return 0;
				}
				else if (strcmp(argv[2], "install") == 0) {
					if (argv[3] == nullptr) { 
						std::cout << "Please, enter the directory of installation script.\n";
						return -1;
					}
					else {
						if (!directoryExists(argv[3])) {
							std::cout << "Directory does not exists or wrong.\n";
							return -1;
						}
						else {
							if (fileExists(argv[3])) {
								patchInstaller.startInstallation(argv[3]);
								return 0;
							}
							else {
								std::cout << "Directory does not exists or wrong.\n";
								return -1;
							}
						}
					}
				}
			}
		}
		catch (std::terminate_handler/* Exception from DBProvider?*/) {
			std::cout << "Parameters were wrong.\n";
			return -1;
		}
	}
}
