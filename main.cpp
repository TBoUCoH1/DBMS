#include "include/Database.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    try {
        Database db("schema.json");
        
        cout << "\nSimple DBMS. Type 'exit' to quit.\n" << endl;
        
        string query;
        while (true) {
            cout << "SQL> ";
            getline(cin, query);
            
            if (query == "exit") {
                break;
            }
            
            if (!query.empty()) {
                db.executeQuery(query);
            }
        }
        
    } catch (const char* e) {
        cerr << "Error: " << e << endl;
        return 1;
    }
    
    return 0;
}
