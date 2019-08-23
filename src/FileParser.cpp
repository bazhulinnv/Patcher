#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/FileParser.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Shared/ParsingTools.h"

FileParser::FileParser() {}
FileParser::~FileParser() {}

/** Check if file DependencyList is incorrect: if any string has more or less than 3 parameters, file considered incorrect. */
bool FileParser::checkInputCorrect(std::string file_name) {
	std::ifstream dependencies;
	dependencies.open(file_name);
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

//public wrapper for private implementation
DBObjs FileParser::getResultOfParsing(std::string file_name) 
{
	FileParser parser;
	return parser.parse(file_name);
}

/** Parsing of file with list of objects. */
DBObjs FileParser::parse(std::string file_name)
{
	DBObjs object_parameters_from_file;
	std::ifstream dependencies;
	dependencies.open(file_name);

	std::string schema("");
	std::string object_name("");
	std::string object_type("");

	//Try to read first string from file
	while (!dependencies.eof()) {
		dependencies >> schema >> object_name >> object_type;
		if (!schema.empty() && !object_name.empty() && !object_type.empty()) {
			object_parameters_from_file.emplace_back(schema, object_name, object_type);
		}
		schema = ""; object_name = ""; object_type = "";
	}

	dependencies.close();
	return object_parameters_from_file;
}