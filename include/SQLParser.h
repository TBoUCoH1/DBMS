#ifndef SQLPARSER_H
#define SQLPARSER_H

#include <string>
#include "Vector.h"

enum class SQLCommandType {
    SELECT,
    INSERT,
    DELETE_CMD,
    UNKNOWN
};

struct SQLCommand {
    SQLCommandType type;
    Vector<std::string> columns;
    Vector<std::string> tables;
    Vector<std::string> values;
    std::string whereClause;
};

class SQLParser {
public:
    static SQLCommand parse(const std::string& query);
};

#endif
