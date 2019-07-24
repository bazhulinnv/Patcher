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
	if ((objectName != "") && (objectType != "")) {
		while (getline(dependencies, buffer)) {
			objectParametersFromFile.emplace_back(scheme, objectName, objectType);
			dependencies >> scheme >> objectName >> objectType;
		}
	}

	dependencies.close();
	return objectParametersFromFile;
}


