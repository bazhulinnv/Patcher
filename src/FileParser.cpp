#include "PatchInstaller/FileParser.h"
#include <iostream>
#include <fstream>
#include <string>

FileParser::FileParser() {}
FileParser::~FileParser() {}

std::list<std::pair<std::string, std::string>> FileParser::parse(std::string nameOfFile)
{
	std::list<std::pair<std::string, std::string>> objectParametersFromFile;

	std::ifstream dependencies(nameOfFile, std::ios::in);

	std::string buffer("");
	std::string objectName("");
	std::string objectType("");;

	//Try to read first string from file
	dependencies >> objectName >> objectType;
	//std::cout << objectName << objectType << "\n";
	if ((objectName != "") && (objectType != "")) {
		while (getline(dependencies, buffer)) {
			std::pair<std::string, std::string> currentObjectParameters = { objectName, objectType };
			objectParametersFromFile.push_back(currentObjectParameters);
			//std::cout << objectName << objectType << "Added to map\n";
			dependencies >> objectName >> objectType;
		}
	}
	std::pair<std::string, std::string> currentObjectParameters = { objectName, objectType };
	objectParametersFromFile.push_back(currentObjectParameters);

	//std::cout << objectName << objectType << "Added to map\n";

	dependencies.close();
	return objectParametersFromFile;
	//return std::unordered_map<std::string, std::string>();
}


