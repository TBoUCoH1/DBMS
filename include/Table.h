#ifndef TABLE_H
#define TABLE_H

#include <string>
#include "Vector.h"
#include "Map.h"
#include "SQLParser.h"

class Table {
public:
    std::string name;
    Vector<std::string> columns;
    std::string path;
    int tuplesLimit;

    Table();
    Table(const std::string& name, const Vector<std::string>& columns,
          const std::string& basePath, int limit);

    void initialize();
    void insert(const Vector<std::string>& values);
    void deleteRows(const std::string& whereClause);
    
    void select(const SQLCommand& cmd, Vector<std::string>& results, bool isFirst = false);
    void join(const std::string& prevRowStr, Vector<std::string>& combinedResults);
    
    static bool evaluateWhere(const Map& row, const std::string& condition);
    static bool evaluateCondition(const Map& row, const std::string& condition);


private:
    int getNextPK();
    Vector<std::string> getAllCSVFiles();
    int countLinesInFile(const std::string& filePath);
};

#endif