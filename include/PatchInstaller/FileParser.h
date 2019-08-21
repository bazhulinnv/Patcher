#ifndef FILEPARSER_H

#define FILEPARSER_H

#include "PatchInstaller/PatchInstaller.h"

#include <iostream>
#include <unordered_map>

class FileParser {
public:
	FileParser();
	~FileParser();
	//
	bool checkInputCorrect(std::string file_name);
	DBObjects getResultOfParsing(std::string file_name);
private:
	DBObjects parse(std::string file_name);
};
#endif