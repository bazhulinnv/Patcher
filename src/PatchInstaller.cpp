#include <fstream>
#include <iostream>
#include <array>
#include <time.h>
#include <filesystem>
#include <direct.h>

#include "PatchInstaller/PatchInstaller.h"
#include "PatchInstaller/DependenciesChecker.h"
#include "DBProvider/DBProviderLogger.h"

using namespace DBProviderLogger;

PatchInstaller::PatchInstaller() {}
PatchInstaller::~PatchInstaller() {}


/** The function checks the presence of objects in the database according to the list of objects specified in the file. */
bool PatchInstaller::checkObjectsForExistenceFromFile(std::string nameOfFile, DBProvider dbProvider) {
	FileParser fileParser;
	std::list<std::tuple<std::string, std::string, std::string>> objectsNameAndType = fileParser.parse(nameOfFile);
	DependenciesChecker checker;
	bool result = checker.check(objectsNameAndType, dbProvider);
	checker.printExistenceOfEachObject();
	return result;
}


/** When the method starts, the dependency check is considered successful. */
/** Figuring out the operating system. */
#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__))
#  define __WIN32__
#endif

#if defined(__unix__) || defined(__unix) || defined(_UNIXWARE7)
#define __unix__
#endif

// You can use a simple guard class to make sure the buffer is always reset:
struct cerrRedirect
{
	cerrRedirect(std::streambuf *new_buffer) : old(std::cerr.rdbuf(new_buffer)) {}
	~cerrRedirect() { std::cerr.rdbuf(old); }
private:
	std::streambuf *old;
};

struct cout_redirect {
	cout_redirect(std::streambuf * new_buffer)
		: old(std::cout.rdbuf(new_buffer))
	{ }

	~cout_redirect() {
		std::cout.rdbuf(old);
	}

private:
	std::streambuf * old;
};

/*inline bool fileExists() {
	ifstream f("Install.bat");
	return f.good();
}*/

/** When the method starts, the dependency check is considered successful. */
bool PatchInstaller::startInstallation(char* directory) {
#if (defined(__WIN32__))
	chdir(directory);
	std::array<char, 128> buffer;
	std::string result;

	std::stringstream awsomeLogBuffer;
	std::stringstream awsomeLogInfoBuffer;
	auto *customRedirect = new cerrRedirect(awsomeLogBuffer.rdbuf());
	auto *customRedirect1 = new cout_redirect(awsomeLogInfoBuffer.rdbuf());
	std::cerr << "ERROR" << std::endl;
	std::cout << "INFO" << std::endl;
	std::string awsomeErrors = awsomeLogBuffer.str();
	std::string info = awsomeLogInfoBuffer.str();

	FILE* pipe = _popen("Install.bat", "r");
	if (!pipe)
	{
		std::cerr << "Couldn't start command." << std::endl;
		return false;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {};
	awsomeErrors = awsomeLogBuffer.str();
	info = awsomeLogInfoBuffer.str();
	std::cout << awsomeErrors << std::endl;
	std::cout << "####" << info << "###" << std::endl;
	delete customRedirect;
	delete customRedirect1;
	auto returnCode = _pclose(pipe);
#endif
#if (defined(__unix__)) 
	FILE* pipe = popen("Install.bat", "r");
	if (!pipe)
	{
		std::cerr << "Couldn't start command." << std::endl;
		return 0;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
		result += buffer.data();
	}
	auto returnCode = pclose(pipe);

	std::cout << "!!!" << result << "!!" << std::endl;
	//std::cout << returnCode << std::endl;
#endif
}
