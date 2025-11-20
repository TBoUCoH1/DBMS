#include "../include/Utils.h"
#include "../include/Map.h"
#include <sys/stat.h>
#include <cctype>
#include <algorithm>
#include <sstream>

using namespace std;

// по символу
Vector<string> split(const string& str, char delimiter) {
    Vector<string> tokens;
    string current;
    stringstream ss(str);
    while (getline(ss, current, delimiter)) {
        tokens.push_back(trim(current));
    }
    return tokens;
}
// по строке
Vector<string> split(const string& str, const string& delimiter) {
    Vector<string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != string::npos) {
        tokens.push_back(trim(str.substr(start, end - start)));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(trim(str.substr(start, end)));
    return tokens;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool fileExists(const string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

string toUpper(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

string join(const Vector<string>& vec, const string& delimiter) {
    string result = "";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i < vec.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

long stol(const string& str) {
    return atol(str.c_str());
}

// сериализация и десереализация
string serializeRow(const Map& row) {
    string res;
    for (Map::Iterator it = row.begin(); it != row.end(); ++it) {
        res += it -> key + ":" + it->value + ";";
    }
    return res;
}


Map deserializeRow(const string& str) {
    Map row;
    Vector<string> pairs = split(str, ';');
    for(size_t i = 0; i < pairs.size(); ++i) {
        if(pairs[i].empty()) continue;
        size_t colonPos = pairs[i].find(':');
        if(colonPos != string::npos) {
            string key = pairs[i].substr(0, colonPos);
            string value = pairs[i].substr(colonPos + 1);
            row[key] = value;
        }
    }
    return row;
}