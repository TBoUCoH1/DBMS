#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "Table.h"
#include "SQLParser.h"
#include "Map.h"
#include "Vector.h"

class Database {
public:
    Database(const std::string& schemaPath);
    ~Database();
    void executeQuery(const std::string& query);

private:
    std::string schemaName;
    int tuplesLimit;
    Map tables;
    Vector<Table> tableObjects; // храним сами объекты

    void loadSchema(const std::string& schemaPath);
    void executeSelect(const SQLCommand& cmd);
    void executeInsert(const SQLCommand& cmd);
    void executeDelete(const SQLCommand& cmd);
    Table* getTable(const std::string& name);
};

#endif