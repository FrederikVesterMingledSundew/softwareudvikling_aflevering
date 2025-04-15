#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include "character.h"

class sqlDB
{
public:
    sqlDB(const std::string &dbFileName);
    ~sqlDB() {
        if (mSqlDB) {
            sqlite3_close(mSqlDB);
        }
    };

    bool isOpen() const { return mSqlDB != nullptr; }

    bool checkUserTable(const int heroNameLength);

    int addNewHero(std::string &name);

    bool searchForHeroes(std::vector<character> &buffer) ;

    bool saveHero(const character &hero);

private:
    std::string mFile;
    sqlite3* mSqlDB = nullptr;
    int rc = {};
};

#endif // SQLDATABASE_H
