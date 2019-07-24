#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <iostream>
#include <unordered_map>

class FileParser {
public:
	FileParser();
	~FileParser();
	static std::list<std::tuple<std::string, std::string, std::string>> parse(std::string nameOfFile);
};
#endif