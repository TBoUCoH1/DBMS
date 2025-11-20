#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include "Vector.h"

class FileManager {
public:
    static bool createDirectory(const std::string& path);
    static Vector<std::string> readCSV(const std::string& filePath);
    static void writeCSV(const std::string& filePath, const Vector<std::string>& lines);
    static void appendCSV(const std::string& filePath, const std::string& line);
    static int readSequence(const std::string& filePath);
    static void writeSequence(const std::string& filePath, int value);
    static bool acquireLock(const std::string& tablePath);
    static void releaseLock(const std::string& tablePath);
};

#endif
