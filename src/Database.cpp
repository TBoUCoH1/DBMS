#include "../include/Database.h"
#include "../include/FileManager.h"
#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Database::Database(const string& schemaPath) : tuplesLimit(1000) {
    loadSchema(schemaPath);
}

Database::~Database() {
}

void Database::loadSchema(const string& schemaPath) {

    ifstream file(schemaPath);
    if (!file.is_open()) {
        throw "Cannot open schema file";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    string jsonStr = buffer.str();

    // парсинг дзона
    size_t namePos = jsonStr.find("\"name\"");
    if (namePos != string::npos) {
        size_t colonPos = jsonStr.find(":", namePos);
        size_t quoteStart = jsonStr.find("\"", colonPos) + 1;
        size_t quoteEnd = jsonStr.find("\"", quoteStart);
        schemaName = jsonStr.substr(quoteStart, quoteEnd - quoteStart);
    }

    size_t limitPos = jsonStr.find("\"tuples_limit\"");
    if (limitPos != string::npos) {
        size_t colonPos = jsonStr.find(":", limitPos);
        tuplesLimit = stoi(trim(jsonStr.substr(colonPos + 1)));
    }

    FileManager::createDirectory(schemaName);

    size_t structPos = jsonStr.find("\"structure\"");
    if (structPos != string::npos) {
        size_t openBrace = jsonStr.find("{", structPos);
        size_t closeBrace = jsonStr.find("}", openBrace);
        string structureStr = jsonStr.substr(openBrace + 1, closeBrace - openBrace - 1);

        size_t currentPos = 0;
        while(currentPos < structureStr.length()) {
            // сами таблицы
            size_t tableNameStart = structureStr.find('"', currentPos);
            if(tableNameStart == string::npos) break;
            size_t tableNameEnd = structureStr.find('"', tableNameStart + 1);
            string tableName = structureStr.substr(tableNameStart + 1, tableNameEnd - tableNameStart - 1);
            // колонки
            size_t arrayStart = structureStr.find('[', tableNameEnd);
            size_t arrayEnd = structureStr.find(']', arrayStart);
            string columnsArrayStr = structureStr.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

            Vector<string> columns = split(columnsArrayStr, ','); // массив из сплитованой строки
            for(size_t i = 0; i < columns.size(); ++i) {
                columns[i] = trim(columns[i]);
                columns[i] = columns[i].substr(1, columns[i].length() - 2); // убираем кавычки
            }

            Table table(tableName, columns, schemaName, tuplesLimit);
            table.initialize();
            
            tableObjects.push_back(table);
            tables[tableName] = to_string(tableObjects.size() - 1);

            currentPos = arrayEnd + 1;
        }
    }

    cout << "Database '" << schemaName << "' initialized with "
         << tableObjects.size() << " tables." << endl;
}

Table* Database::getTable(const string& name) {
    if (!tables.count(name)) {
        cerr << "Error: Table '" << name << "' not found." << endl;
        return nullptr;
    }
    long index = stol(tables[name]);
    return &tableObjects[index];
}

void Database::executeQuery(const string& query) {
    SQLCommand cmd = SQLParser::parse(query);

    switch (cmd.type) {
        case SQLCommandType::SELECT:
            executeSelect(cmd);
            break;
        case SQLCommandType::INSERT:
            executeInsert(cmd);
            break;
        case SQLCommandType::DELETE_CMD:
            executeDelete(cmd);
            break;
        default:
            cerr << "Unknown or invalid command" << endl;
    }
}

void Database::executeSelect(const SQLCommand& cmd) {
    if (cmd.tables.empty()) return;

    for (size_t i = 0; i < cmd.tables.size(); ++i) {
        if (getTable(cmd.tables[i]) == nullptr) return;
    }

    for (size_t i = 0; i < cmd.columns.size(); i++) {
        cout << cmd.columns[i] << "\t";
    }
    cout << endl;

    Vector<string> results;
    Table* firstTable = getTable(cmd.tables[0]);
    firstTable -> select(cmd, results, true); // рекурсивный сбор данных

    // остальные таблицы
    for (size_t i = 1; i < cmd.tables.size(); ++i) {
        Vector<string> tempResults;
        Table* nextTable = getTable(cmd.tables[i]);
        
        // декартово произведение строк
        for(size_t j = 0; j < results.size(); ++j){
            nextTable -> join(results[j], tempResults);
        }
        results = tempResults;
    }
    
    for(size_t i = 0; i < results.size(); ++i){
        Map row = deserializeRow(results[i]);
        if(cmd.whereClause.empty() || Table::evaluateWhere(row, cmd.whereClause)){
            string outputRow;
            for(size_t c = 0; c < cmd.columns.size(); ++c){
                if(cmd.columns[c] == "*"){
                    outputRow = results[i]; // всё выводим
                    break;
                }
                if(row.count(cmd.columns[c])){
                    outputRow += row.at(cmd.columns[c]) + "\t";
                }
            }
            cout << trim(outputRow) << endl;
        }
    }
}


void Database::executeInsert(const SQLCommand& cmd) {
    if (cmd.tables.empty()) return;
    Table* table = getTable(cmd.tables[0]);
    if (table) {
        table -> insert(cmd.values);
        cout << "Row inserted successfully" << endl;
    }
}

void Database::executeDelete(const SQLCommand& cmd) {
    if (cmd.tables.empty()) return;
    Table* table = getTable(cmd.tables[0]);
    if (table) {
        table -> deleteRows(cmd.whereClause);
        cout << "Rows deleted successfully" << endl;
    }
}
