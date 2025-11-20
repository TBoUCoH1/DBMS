#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "Vector.h"
#include "Map.h"

Vector<std::string> split(const std::string& str, char delimiter);
Vector<std::string> split(const std::string& str, const std::string& delimiter);

std::string trim(const std::string& str);
bool fileExists(const std::string& path);
std::string toUpper(const std::string& str);
std::string join(const Vector<std::string>& vec, const std::string& delimiter);

long std::stol(const std::string& str);

std::string serializeRow(const Map& row);
Map deserializeRow(const std::string& str);

#endif