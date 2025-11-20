#include "../include/SQLParser.h"
#include "../include/Utils.h"
#include <iostream>

using namespace std;

SQLCommand SQLParser::parse(const string& query) {
    SQLCommand cmd;
    string upperQuery = toUpper(query);

    size_t selectPos = upperQuery.find("SELECT");
    size_t insertPos = upperQuery.find("INSERT");
    size_t deletePos = upperQuery.find("DELETE");

    if (selectPos != string::npos) {
        cmd.type = SQLCommandType::SELECT;
        size_t fromPos = upperQuery.find("FROM", selectPos);
        if (fromPos == string::npos) { cmd.type = SQLCommandType::UNKNOWN; return cmd; }

        string columnsStr = trim(query.substr(selectPos + 6, fromPos - (selectPos + 6)));
        cmd.columns = split(columnsStr, ',');

        size_t wherePos = upperQuery.find("WHERE", fromPos);
        string tablesStr;
        if (wherePos != string::npos) {
            tablesStr = trim(query.substr(fromPos + 4, wherePos - (fromPos + 4)));
            cmd.whereClause = trim(query.substr(wherePos + 5));
        } else {
            tablesStr = trim(query.substr(fromPos + 4));
        }
        cmd.tables = split(tablesStr, ',');

    } else if (insertPos != string::npos) {
        cmd.type = SQLCommandType::INSERT;
        size_t intoPos = upperQuery.find("INTO", insertPos);
        size_t valuesPos = upperQuery.find("VALUES", intoPos);
        if (intoPos == string::npos || valuesPos == string::npos) { cmd.type = SQLCommandType::UNKNOWN; return cmd; }

        string tableName = trim(query.substr(intoPos + 4, valuesPos - (intoPos + 4)));
        cmd.tables.push_back(tableName);

        size_t openParen = query.find('(', valuesPos);
        size_t closeParen = query.rfind(')');
        if (openParen == string::npos || closeParen == string::npos) { cmd.type = SQLCommandType::UNKNOWN; return cmd; }

        string valuesStr = query.substr(openParen + 1, closeParen - openParen - 1);
        cmd.values = split(valuesStr, ',');
        for (size_t i = 0; i < cmd.values.size(); i++) {
            string val = trim(cmd.values[i]);
            if (val.front() == '\'' && val.back() == '\'') {
                cmd.values[i] = val.substr(1, val.size() - 2);
            } else {
                 cmd.values[i] = val;
            }
        }

    } else if (deletePos != string::npos) {
        cmd.type = SQLCommandType::DELETE_CMD;
        size_t fromPos = upperQuery.find("FROM", deletePos);
        if (fromPos == string::npos) { cmd.type = SQLCommandType::UNKNOWN; return cmd; }

        size_t wherePos = upperQuery.find("WHERE", fromPos);
        if (wherePos == string::npos) {
             string tableName = trim(query.substr(fromPos + 4));
             cmd.tables.push_back(tableName);
             cmd.whereClause = "";
        } else {
            string tableName = trim(query.substr(fromPos + 4, wherePos - (fromPos + 4)));
            cmd.tables.push_back(tableName);
            cmd.whereClause = trim(query.substr(wherePos + 5));
        }

    } else {
        cmd.type = SQLCommandType::UNKNOWN;
    }

    return cmd;
}