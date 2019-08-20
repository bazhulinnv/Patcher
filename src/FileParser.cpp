#include "PatchInstaller/FileParser.h"
#include "PatchInstaller/PatchInstaller.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Shared/ParsingTools.h"

FileParser::FileParser() {}
FileParser::~FileParser() {}

/** Check if file DependencyList is incorrect: if any string has more or less than 3 parameters, file considered incorrect. */
bool FileParser::checkInputCorrect(std::string nameOfFile) {
	std::ifstream dependencies;
	dependencies.open(nameOfFile);
	while (!dependencies.eof()) {
		std::string buffer;
		std::getline(dependencies, buffer);

		if (!buffer.empty()) {
			std::vector<std::string> parameters = ParsingTools::SplitToVector(buffer, " ");
			if (parameters.size() != 3) {
				return false;
			}		
		}
	}
	dependencies.close();
	return true;
}

//public wrapper for private implementation
DBObjects FileParser::getResultOfParsing(std::string nameOfFile) 
{
	FileParser parser;
	return parser.parse(nameOfFile);
}

/** Parsing of file with list of objects. */
DBObjects FileParser::parse(std::string nameOfFile)
{
	DBObjects objectParametersFromFile;
	std::ifstream dependencies;
	dependencies.open(nameOfFile);

	std::string schema("");
	std::string objectName("");
	std::string objectType("");

	//Try to read first string from file
	while (!dependencies.eof()) {
		dependencies >> schema >> objectName >> objectType;
		if (!schema.empty() && !objectName.empty() && !objectType.empty()) {
			objectParametersFromFile.emplace_back(schema, objectName, objectType);
		}
		schema = ""; objectName = ""; objectType = "";
	}

	dependencies.close();
	return objectParametersFromFile;
}


