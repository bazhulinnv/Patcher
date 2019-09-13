#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "PatchInstaller/PatchInstaller.h"

class FileParser {
public:
	FileParser();
	~FileParser();
	//
	bool checkInputCorrect(std::string file_name);
	DBObjs getResultOfParsing(std::string file_name);
private:
	DBObjs parse(std::string file_name);
};
#endif