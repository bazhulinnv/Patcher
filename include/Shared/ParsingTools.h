#ifndef PARSINGTOOLS_H
#define PARSINGTOOLS_H

#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace ParsingTools {
std::string Interpolate(std::string input, const std::string &replacement,
                        const std::string &to_be_replaced = "${}");

std::string InterpolateAll(const std::string &input,
                           std::queue<std::string> replacements,
                           const std::string &to_be_replaced = "${}");

std::string InterpolateAll(const std::string &input,
                           std::vector<std::string> replacements,
                           const std::string &to_be_replaced = "${}");

// std::string string("hello $name");
// bool is_ok = Replace(string, "Somename", "$name");
bool Replace(std::string &input, const std::string &replacement,
             const std::string &to_be_replaced = "${}");

void ReplaceAll(std::string &input, const std::string &replacement,
                const std::string &to_be_replaced = "${}");

std::vector<std::string> SplitToVector(std::string input,
                                       const std::string &delimiter);

std::string JoinAsString(const std::vector<std::string> &input,
                         const char *delimiter = "");

std::string ParseCredentials(const std::string &pg_login);

std::pair<bool, std::string> TryParseCredentials(const std::string &pg_login);
} // namespace ParsingTools

#endif