#include "SQLdatabase.h"
#include <iostream>

SQLdb::SQLdb(const std::string& dbFileName) : mFile(dbFileName), sqlDB(nullptr) {
    int exit = sqlite3_open(mFile.c_str(), &sqlDB);
    if (exit) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(sqlDB) << std::endl;
        if (sqlDB) {
            sqlite3_close(sqlDB);
            sqlDB = nullptr;
        }
    }
}


