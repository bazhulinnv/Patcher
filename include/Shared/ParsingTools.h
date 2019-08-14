#include <string>
#include <vector>
#include <utility>
#include <queue>

namespace ParsingTools
{
	std::string interpolate(std::string input, const std::string& replacement, const std::string toBeReplaced = "${}");

	std::string interpolateAll(const std::string& input, std::queue<std::string> replacements, const std::string toBeReplaced = "${}");

	std::string interpolateAll(const std::string& input, std::vector<std::string> replacements, const std::string toBeReplaced = "${}");

	// std::string string("hello $name");
	// bool is_ok = replace(string, "Somename", "$name");
	bool replace(std::string& input, const std::string& replacement, const std::string& toBeReplaced = "${}");

	void replaceAll(std::string& input, const std::string& replacement, const std::string& toBeReplaced = "${}");

	std::vector<std::string> splitToVector(std::string input, const std::string& delimiter);

	std::string joinAsString(const std::vector<std::string>& input, const char* delimiter = "");

	std::string parseCredentials(const std::string& pgLogin);

	std::pair<bool, std::string> tryParseCredentials(const std::string& pgLogin);
}
