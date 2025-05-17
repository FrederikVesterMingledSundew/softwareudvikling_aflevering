#include "SQLdatabase.h"
#include <iostream>
#include <cstring>

sqlDB::sqlDB(const std::string& dbFileName) : mFile(dbFileName) {
    rc = sqlite3_open(mFile.c_str(), &mSqlDB);
    if (rc) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(mSqlDB) << std::endl;
        if (mSqlDB) {
            sqlite3_close(mSqlDB);
            mSqlDB = nullptr;
        }
    }
}

bool sqlDB::checkUserTable() {

    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }
    std::string sql = "CREATE TABLE IF NOT EXISTS heroes(\
        ID INTEGER PRIMARY KEY AUTOINCREMENT,\
        HERO TEXT NOT NULL,\
        XP INTEGER NOT NULL,\
        LEVEL INTEGER NOT NULL,\
        COINS INTEGER NOT NULL,\
        HP INTEGER NOT NULL,\
        STRENGTH INTEGER NOT NULL\
        );";

        sql += "\
        CREATE TABLE IF NOT EXISTS heroStats (\
        HERO_ID	INTEGER NOT NULL UNIQUE,\
        NAME	TEXT NOT NULL,\
        KILLS	INTEGER NOT NULL DEFAULT 0,\
        XP_GAINED_FROM_KILLS	INTEGER NOT NULL DEFAULT 0,\
        PRIMARY KEY(HERO_ID)\
        );";

        sql += "\
        CREATE TABLE IF NOT EXISTS weaponsTypes (\
        NAME	TEXT NOT NULL,\
        ID	INTEGER NOT NULL DEFAULT 0,\
        MODIFIER	INTEGER NOT NULL DEFAULT 0,\
        DURABILITY	INTEGER NOT NULL DEFAULT 0,\
        PRICE	INTEGER NOT NULL DEFAULT 1000,\
        PRIMARY KEY(ID AUTOINCREMENT)\
        );";

        sql += "\
        CREATE TABLE IF NOT EXISTS activeWeapons (\
        OBJECT_ID	INTEGER NOT NULL,\
        WEAPON_TYPE_ID	INTEGER NOT NULL,\
        PLAYER_ID	INTEGER NOT NULL,\
        HITS	INTEGER NOT NULL DEFAULT 0,\
        PRIMARY KEY(OBJECT_ID AUTOINCREMENT)\
        );";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(mSqlDB, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;

    } else {
        std::cout << "Tables created successfully" << std::endl;
        return true;
    }
}

int sqlDB::addNewHero(std::string &name) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return -1;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = "INSERT INTO heroes (HERO, XP, LEVEL, COINS, HP, STRENGTH) VALUES (?, 0, 1, 0, ?, ?);";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, START_HP);
    sqlite3_bind_int(stmt, 3, START_STRENGTH);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR]: Failed to execute statement: \n" << sqlite3_errmsg(mSqlDB) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    // Get the last inserted row ID
    int lastInsertID = static_cast<int>(sqlite3_last_insert_rowid(mSqlDB));
    std::cout << "[INFO]: Hero inserted successfully with ID: " << lastInsertID << std::endl;

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
    return lastInsertID;
}

bool sqlDB::searchForHeroes(std::vector<character> &buffer) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* selectSQL = "SELECT `ID`, `HERO`, `XP`, `LEVEL`, `COINS`, `HP`, `STRENGTH` FROM heroes";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(mSqlDB, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare SQL statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    // Clear the buffer to ensure it's fresh
    buffer.clear();

    // Iterate through the results of the query
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve and assign values from the database row
        int ID = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); // HERO
        int xp = sqlite3_column_int(stmt, 2);                                    // XP
        int level = sqlite3_column_int(stmt, 3);                                 // LEVEL
        int coins = sqlite3_column_int(stmt, 4);                                 // COINS
        int hp = sqlite3_column_int(stmt, 5);                                    // HP
        int strength = sqlite3_column_int(stmt, 6);                              // STRENGTH
        character hero(name, ID, xp, level, coins, hp, strength);

        // Add the hero to the buffer
        buffer.push_back(hero);
    }

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);

    std::cout << "[INFO]: Successfully retrieved heroes from the database." << std::endl;
    return true;
}

bool sqlDB::saveHero(const character &hero) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = "UPDATE heroes SET `XP` = ?, `LEVEL` = ?, `COINS` = ?, `HP` = ?, `STRENGTH` = ? WHERE `ID` = ?;";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, hero.getXP());
    sqlite3_bind_int(stmt, 2, hero.getLvl());
    sqlite3_bind_int(stmt, 3, hero.getCoins());
    sqlite3_bind_int(stmt, 4, hero.getHp());
    sqlite3_bind_int(stmt, 5, hero.getStrength());
    sqlite3_bind_int(stmt, 6, hero.getId());

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR]: Failed to execute statement: \n" << sqlite3_errmsg(mSqlDB) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }


    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
    return true;
}

bool sqlDB::killHero(const character &hero) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = "DELETE FROM heroes WHERE `ID` = ? LIMIT 1;";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, hero.getId());

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR]: Failed to execute statement: \n" << sqlite3_errmsg(mSqlDB) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }


    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
    return true;
}

bool sqlDB::loadWeaponShop(const character &hero) {
    return true;
}
