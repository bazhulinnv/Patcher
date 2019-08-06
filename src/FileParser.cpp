#include "PatchInstaller/FileParser.h"
#include "PatchInstaller/PatchInstaller.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Shared/ParsingTools.h"

FileParser::FileParser() {}
FileParser::~FileParser() {}

bool FileParser::checkInputCorrect(std::string nameOfFile) {
	std::ifstream dependencies;
	dependencies.open(nameOfFile);
	while (!dependencies.eof()) {
		std::string buffer;
		std::getline(dependencies, buffer);

		if (!buffer.empty()) {
			std::vector<std::string> parameters = ParsingTools::splitToVector(buffer, " ");
			if (parameters.size() != 3) {
				return false;
			}		
		}
	}
	dependencies.close();
	return true;
}

DBObjects FileParser::getResultOfParsing(std::string nameOfFile) 
{
	FileParser parser;
	return parser.parse(nameOfFile);
}


DBObjects FileParser::parse(std::string nameOfFile)
{
	DBObjects objectParametersFromFile;
	std::ifstream dependencies;
	dependencies.open(nameOfFile);

	std::string scheme("");
	std::string objectName("");
	std::string objectType("");

	//Try to read first string from file
	while (!dependencies.eof()) {
		dependencies >> scheme >> objectName >> objectType;
		if (!scheme.empty() && !objectName.empty() && !objectType.empty()) {
			objectParametersFromFile.emplace_back(scheme, objectName, objectType);
		}
		scheme = ""; objectName = ""; objectType = "";
	}

	dependencies.close();
	return objectParametersFromFile;
}


