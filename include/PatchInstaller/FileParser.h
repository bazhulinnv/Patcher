#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "PatchInstaller/PatchInstaller.h"

#include <iostream>
#include <unordered_map>

class FileParser {
public:
	FileParser();
	~FileParser();
	static DBObjects getResultOfParsing(std::string nameOfFile);
private:
	DBObjects parse(std::string nameOfFile);
};
#endif