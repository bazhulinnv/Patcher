#include "PatchInstaller/FileParser.h"
#include <iostream>
#include <fstream>
#include <string>

FileParser::FileParser() {}
FileParser::~FileParser() {}

std::list<std::tuple<std::string, std::string, std::string>> FileParser::parse(std::string nameOfFile)
{
	std::list<std::tuple<std::string, std::string, std::string>> objectParametersFromFile;
	std::ifstream dependencies(nameOfFile, std::ios::in);

	std::string buffer("");
	std::string scheme("");
	std::string objectName("");
	std::string objectType("");

	//Try to read first string from file
	dependencies >> scheme >>objectName >> objectType;
	//std::cout << scheme << objectName << objectType << "\n\n";
	if ((objectName != "") && (objectType != "")) {
		while (getline(dependencies, buffer)) {
			//std::pair<std::string, std::string> currentObjectParameters = { objectName, objectType };
			objectParametersFromFile.emplace_back(scheme, objectName, objectType);
			//std::cout << scheme << objectName << objectType << " added\n";
			dependencies >> scheme >> objectName >> objectType;
		}
	}
	//std::cout << "\n" << scheme << objectName << objectType << "added\n";

	dependencies.close();
	return objectParametersFromFile;
}


