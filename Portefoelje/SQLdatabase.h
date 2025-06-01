#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include "character.h"

//Stats
struct statEntry {
    int id = 0, kills = 0, xpFromKills = 0;
    std::string name {};
};


/*
 * Alt SQL foregår igennem denne klasse
 *
 */

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

    bool checkUserTable();

    int addNewHero(std::string &name);

    bool searchForHeroes(std::vector<character> &buffer) ;

    bool saveHero(const character &hero);

    bool killHero(const character &hero);

    bool loadWeaponShop(const character &hero, std::vector<Weapon> &shopItems);

    bool addWeaponType(std::string name, int durability, int modifier, int price);

    bool addToStats(const character &hero, int kills, int XP);

    bool loadStats(std::vector<statEntry> &stats);

private:
    std::string mFile;
    sqlite3* mSqlDB = nullptr;
    int rc = {};
};

#endif // SQLDATABASE_H
