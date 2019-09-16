#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "PatchInstaller/PatchInstaller.h"

class FileParser {
public:
  FileParser();
  ~FileParser();

  /** Check if file DependencyList is incorrect: if any string has more or less
   * than 3 parameters, file considered incorrect. */
  bool checkInputCorrect(const std::string &file_name) const;
  DBObjs getResultOfParsing(const std::string &file_name);

private:
  DBObjs parse(const std::string &file_name);
};
#endif