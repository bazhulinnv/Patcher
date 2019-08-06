#include <string>
#include <vector>
#include <utility>
#include <queue>

namespace ParsingTools
{	
	std::string interpolate(std::string original, const std::string& replacement, std::string toBeReplaced = "${}");

	std::string interpolateAll(const std::string& str, std::queue<std::string> replacements, std::string toBeReplaced = "${}");

	std::string interpolateAll(const std::string& str, std::vector<std::string> replacements, std::string toBeReplaced = "${}");

	// std::string string("hello $name");
	// replace(string, "$name", "Somename");
	bool replace(std::string& str, const std::string& from, const std::string& to);
	
	void replaceAll(std::string& str, const std::string& from, const std::string& to);
	
	std::vector<std::string> splitToVector(std::string str, const std::string& delimiter);

	std::string joinAsStrings(const std::vector<std::string>& vec, const char* delimiter);
	
	std::pair<std::vector<std::string>, std::string> parseCredentials(std::string& input);
}
