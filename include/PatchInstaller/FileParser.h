#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "PatchInstaller/PatchInstaller.h"

#include <iostream>
#include <unordered_map>

class FileParser {
public:
	FileParser();
	~FileParser();
	static DBProviderListParameters getResultOfParsing(std::string nameOfFile);
private:
	DBProviderListParameters parse(std::string nameOfFile);
};
#endif