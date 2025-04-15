#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <sqlite3.h>
#include <string>

class SQLdb
{
public:
    SQLdb(const std::string &dbFileName);
    ~SQLdb() {
        if (sqlDB) {
            sqlite3_close(sqlDB);
        }
    };

    bool isOpen() const { return sqlDB != nullptr; }

    std::string mFile;
    sqlite3* sqlDB;

};

#endif // SQLDATABASE_H
