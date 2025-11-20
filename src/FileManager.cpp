#include "../include/FileManager.h"
#include "../include/Utils.h"
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

using namespace std;

bool FileManager::createDirectory(const string& path) {
    return mkdir(path.c_str(), 0777) == 0 || errno == EEXIST;
}

Vector<string> FileManager::readCSV(const string& filePath) {
    Vector<string> lines;
    ifstream file(filePath);
    
    if (!file.is_open()) {
        return lines;
    }
    
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    
    file.close();
    return lines;
}

void FileManager::writeCSV(const string& filePath, const Vector<string>& lines) {
    ofstream file(filePath);
    
    for (size_t i = 0; i < lines.size(); i++) {
        file << lines[i] << "\n";
    }
    
    file.close();
}

void FileManager::appendCSV(const string& filePath, const string& line) {
    ofstream file(filePath, ios::app);
    file << line << "\n";
    file.close();
}

int FileManager::readSequence(const string& filePath) {
    ifstream file(filePath);
    int value = 0;
    
    if (file.is_open()) {
        file >> value;
        file.close();
    }
    
    return value;
}

void FileManager::writeSequence(const string& filePath, int value) {
    ofstream file(filePath);
    file << value;
    file.close();
}

bool FileManager::acquireLock(const string& tablePath) {
    string lockFile = tablePath + "_lock";
    
    if (fileExists(lockFile)) {
        return false;
    }
    
    ofstream file(lockFile);
    file << "locked";
    file.close();
    
    return true;
}

void FileManager::releaseLock(const string& tablePath) {
    string lockFile = tablePath + "_lock";
    remove(lockFile.c_str());
}
