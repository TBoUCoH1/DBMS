#include "../include/Table.h"
#include "../include/FileManager.h"
#include "../include/Utils.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

Table::Table() : tuplesLimit(1000) {}

Table::Table(const string& name, const Vector<string>& columns,
             const string& basePath, int limit)
    : name(name), columns(columns), tuplesLimit(limit) {
    path = basePath + "/" + name;
}

void Table::initialize() {
    FileManager::createDirectory(path);

    Vector<string> allColumns;
    allColumns.push_back(name + "_pk");
    for (size_t i = 0; i < columns.size(); i++) {
        allColumns.push_back(columns[i]);
    }

    string csvPath = path + "/1.csv";
    if (!fileExists(csvPath)) {
        string header = ::join(allColumns, ",");
        Vector<string> lines;
        lines.push_back(header);
        FileManager::writeCSV(csvPath, lines);
    }

    string seqPath = path + "/" + name + "_pk_sequence";
    if (!fileExists(seqPath)) {
        FileManager::writeSequence(seqPath, 1);
    }
}

int Table::getNextPK() {
    string seqPath = path + "/" + name + "_pk_sequence";
    int pk = FileManager::readSequence(seqPath);
    FileManager::writeSequence(seqPath, pk + 1);
    return pk;
}

Vector<string> Table::getAllCSVFiles() {
    Vector<string> files;
    int fileNum = 1;
    while (true) {
        string filePath = path + "/" + to_string(fileNum) + ".csv";
        if (fileExists(filePath)) {
            files.push_back(filePath);
            fileNum++;
        } else {
            break;
        }
    }
    return files;
}

int Table::countLinesInFile(const string& filePath) {
    ifstream file(filePath);
    if(!file.is_open()) return 0;
    
    int count = 0;
    string line;
    // Пропускаем заголовок
    getline(file, line);
    while (getline(file, line)) {
        count++;
    }
    return count;
}

void Table::insert(const Vector<string>& values) {
    string lockPath = path + "/" + name + "_lock";
    if (!FileManager::acquireLock(lockPath)) {
        cerr << "Table is locked!" << endl;
        return;
    }

    string row = to_string(getNextPK());
    for (size_t i = 0; i < values.size(); i++) {
        row += "," + values[i];
    }

    Vector<string> csvFiles = getAllCSVFiles();
    string targetFile;

    if (csvFiles.empty()) {
        targetFile = path + "/1.csv";
        Vector<string> allColumns;
        allColumns.push_back(name + "_pk");
        for(size_t i = 0; i < columns.size(); ++i) allColumns.push_back(columns[i]);
        Vector<string> headerLine;
        headerLine.push_back(::join(allColumns, ","));
        FileManager::writeCSV(targetFile, headerLine);
    } else {
        for (size_t i = 0; i < csvFiles.size(); i++) {
            if (countLinesInFile(csvFiles[i]) < tuplesLimit) {
                targetFile = csvFiles[i];
                break;
            }
        }
    }

    if (targetFile.empty()) {
        int newFileNum = csvFiles.size() + 1;
        targetFile = path + "/" + to_string(newFileNum) + ".csv";

        ifstream firstFile(csvFiles[0]);
        string header;
        if(getline(firstFile, header)){
            Vector<string> headerLine;
            headerLine.push_back(header);
            FileManager::writeCSV(targetFile, headerLine);
        }
        firstFile.close();
    }

    FileManager::appendCSV(targetFile, row);
    FileManager::releaseLock(lockPath);
}

void Table::select(const SQLCommand& cmd, Vector<string>& results, bool isFirst) {
    Vector<string> csvFiles = getAllCSVFiles();
    
    for (size_t f = 0; f < csvFiles.size(); f++) {
        ifstream file(csvFiles[f]);
        if (!file.is_open()) continue;

        string headerLine;
        getline(file, headerLine);
        Vector<string> headers = split(headerLine, ',');

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            Vector<string> values = split(line, ',');
            
            Map row;
            for (size_t j = 0; j < headers.size() && j < values.size(); j++) {
                row[name + "." + headers[j]] = values[j];
            }
            results.push_back(serializeRow(row));
        }
        file.close();
    }
}

// объединение строк при join
void Table::join(const string& prevRowStr, Vector<string>& combinedResults) {
    Vector<string> csvFiles = getAllCSVFiles();

    for (size_t f = 0; f < csvFiles.size(); f++) {
        ifstream file(csvFiles[f]);
        if (!file.is_open()) continue;

        string headerLine;
        getline(file, headerLine);
        Vector<string> headers = split(headerLine, ',');

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            Vector<string> values = split(line, ',');
            
            Map combinedRow = deserializeRow(prevRowStr);
            for (size_t j = 0; j < headers.size() && j < values.size(); j++) {
                combinedRow[name + "." + headers[j]] = values[j];
            }
            combinedResults.push_back(serializeRow(combinedRow));
        }
        file.close();
    }
}


void Table::deleteRows(const string& whereClause) {
    string lockPath = path + "/" + name + "_lock";
    if (!FileManager::acquireLock(lockPath)) {
        cerr << "Table is locked!" << endl;
        return;
    }

    Vector<string> csvFiles = getAllCSVFiles();
    for (size_t f = 0; f < csvFiles.size(); f++) {
        ifstream file(csvFiles[f]);
        if(!file.is_open()) continue;
        
        string headerLine;
        getline(file, headerLine);
        Vector<string> headers = split(headerLine, ',');

        Vector<string> newLines;
        newLines.push_back(headerLine);
        
        string line;
        while(getline(file, line)){
            if(line.empty()) continue;
            Vector<string> values = split(line, ',');

            Map row;
            for (size_t j = 0; j < headers.size() && j < values.size(); j++) {
                row[name + "." + headers[j]] = values[j];
            }
            
            if (!evaluateWhere(row, whereClause)) {
                newLines.push_back(line);
            }
        }
        file.close();
        FileManager::writeCSV(csvFiles[f], newLines);
    }
    FileManager::releaseLock(lockPath);
}

// проверяет условия
bool Table::evaluateWhere(const Map& row, const string& condition) {
    Vector<string> or_parts = split(toUpper(condition), string(" OR "));
    
    for(size_t i = 0; i < or_parts.size(); ++i){
        Vector<string> and_parts = split(or_parts[i], string(" AND "));
        bool and_result = true;
        for(size_t j = 0; j < and_parts.size(); ++j){
            if(!evaluateCondition(row, trim(condition.substr(toUpper(condition).find(and_parts[j]), and_parts[j].length())))){
                and_result = false;
                break;
            }
        }
        if(and_result) return true;
    }
    return false;
}

bool Table::evaluateCondition(const Map& row, const string& condition) {
    Vector<string> parts = split(condition, '=');
    if (parts.size() != 2) return true;

    string left_col = trim(parts[0]);
    string right_val = trim(parts[1]);

    string left_value;
    if(row.count(left_col)) {
        left_value = row.at(left_col);
    } else {
        return false;
    }
    
    if (right_val.front() == '\'' && right_val.back() == '\'') {
        return left_value == right_val.substr(1, right_val.size() - 2);
    } else {
        if (row.count(right_val)) {
            return left_value == row.at(right_val);
        }
        return false;
    }
}