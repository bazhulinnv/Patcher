#include "Shared/ParsingTools.h"

#include <iterator>
#include <sstream>

using namespace std;

string ParsingTools::Interpolate(string input, const string &replacement,
                                 const string &to_be_replaced) {
  string new_string = input.replace(input.find(to_be_replaced),
                                    to_be_replaced.length(), replacement);
  return new_string;
}

string ParsingTools::InterpolateAll(const string &input,
                                    queue<string> replacements,
                                    const string &to_be_replaced) {
  if (input.empty()) {
    return string();
  }

  string res = input;
  size_t start_pos = 0;

  while ((start_pos = res.find(to_be_replaced, start_pos)) != string::npos &&
         !replacements.empty()) {
    res.replace(start_pos, to_be_replaced.length(), replacements.front());
    start_pos += replacements.front().length(); // In case 'to' contains 'from',
                                                // like replacing 'x' with 'yx'
    replacements.pop();
  }

  return res;
}

string ParsingTools::InterpolateAll(const string &input,
                                    vector<string> replacements,
                                    const string &to_be_replaced) {
  if (to_be_replaced.empty() || input.empty()) {
    return string();
  }

  string res = input;
  size_t start_pos = 0;

  while ((start_pos = res.find(to_be_replaced, start_pos)) != string::npos &&
         !replacements.empty()) {
    res.replace(start_pos, to_be_replaced.length(), replacements.front());
    start_pos += replacements.front().length();
    replacements.erase(replacements.begin());
  }

  return res;
}

// string string("hello $name");
// bool is_ok = Replace(string, "Somename", "$name");
bool ParsingTools::Replace(string &input, const string &replacement,
                           const string &to_be_replaced) {
  const size_t start_pos = input.find(to_be_replaced);

  if (start_pos == string::npos) {
    return false;
  }

  input.replace(start_pos, to_be_replaced.length(), replacement);
  return true;
}

void ParsingTools::ReplaceAll(string &input, const string &replacement,
                              const string &to_be_replaced) {
  if (to_be_replaced.empty()) {
    return;
  }

  size_t start_pos = 0;
  while ((start_pos = input.find(to_be_replaced, start_pos)) != string::npos) {
    input.replace(start_pos, to_be_replaced.length(), replacement);
    start_pos += replacement.length(); // In case 'to' contains 'from', like
                                       // replacing 'x' with 'yx'
  }
}

vector<string> ParsingTools::SplitToVector(string input,
                                           const string &delimiter) {
  // In case empty string - result is empty vector<string>
  vector<string> result;

  while (!input.empty()) {
    size_t index = input.find(delimiter);

    if (index != string::npos) {
      result.push_back(input.substr(0, index));
      input = input.substr(index + delimiter.size());

      if (input.empty()) {
        result.push_back(input);
      }
    } else {
      result.push_back(input);
      input = "";
    }
  }

  return result;
}

string ParsingTools::JoinAsString(const vector<string> &input,
                                  const char *delimiter) {

  if (input.size() == 1)
    return input[0];
  // If vector is empty, return empty string
  if (input.empty())
    return string("");

  stringstream res;
  copy(input.begin(), input.end(), ostream_iterator<string>(res, delimiter));
  return res.str();
}

string ParsingTools::ParseCredentials(const string &pg_login) {
  vector<string> params = {
      "hostaddr=${}", "port=${}", "dbname=${}", "user=${}", "password=${}",
  };

  vector<string> values = SplitToVector(pg_login, ":");
  if (values[0] == "localhost" || values[0] == "loopback")
    values[0] = "127.0.0.1";

  vector<string> items;
  items.reserve(params.size());
  for (int i = 0; i < params.size(); ++i) {
    items.push_back(Interpolate(params[i], values[i]));
  }

  return JoinAsString(items, " ");
}

pair<bool, string> ParsingTools::TryParseCredentials(const string &pg_login) {
  string result;
  vector<string> parsed;
  vector<string> params = {
      "hostaddr=${}", "port=${}", "dbname=${}", "user=${}", "password=${}",
  };

  try {
    vector<string> values = SplitToVector(pg_login, ":");
    if (values[0] == "localhost" || values[0] == "loopback")
      values[0] = "127.0.0.1";

    for (int i = 0; i < params.size(); ++i) {
      parsed.push_back(Interpolate(params[i], values[i]));
    }
  } catch (const exception &) {
    return make_pair(false, JoinAsString(parsed, " "));
  }

  // If parsing succeeded
  // convert to single string using stringstream
  return make_pair(true, JoinAsString(parsed, " "));
}
