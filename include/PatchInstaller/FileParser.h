#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "PatchInstaller/PatchInstaller.h"

class FileParser {
public:
	FileParser();
	~FileParser();
	//
	bool checkInputCorrect(const std::string& file_name);
	DBObjs getResultOfParsing(std::string file_name);
private:
	DBObjs parse(const std::string& file_name);
};
#endif