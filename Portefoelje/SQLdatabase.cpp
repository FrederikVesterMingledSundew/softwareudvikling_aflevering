#include "SQLdatabase.h"
#include <iostream>

//Delete libs
#include <cstring>
#include <chrono>
#include <thread>


#include "weaponfactory.h"


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
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS weaponsTypes (
        NAME	TEXT NOT NULL,
        ID	INTEGER,
        MODIFIER	INTEGER NOT NULL DEFAULT 0,
        DURABILITY	INTEGER NOT NULL DEFAULT 0,
        PRICE	INTEGER NOT NULL DEFAULT 1000,
        PRIMARY KEY(ID AUTOINCREMENT)
        );)";

        sql += R"(CREATE TABLE IF NOT EXISTS heroes(
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        HERO TEXT NOT NULL,
        XP INTEGER NOT NULL,
        LEVEL INTEGER NOT NULL,
        COINS INTEGER NOT NULL,
        HP INTEGER NOT NULL,
        STRENGTH INTEGER NOT NULL,
        WEAPON_TYPE_ID INTEGER,
        WEAPON_HITS	INTEGER NOT NULL DEFAULT 0,
        FOREIGN KEY(WEAPON_TYPE_ID) REFERENCES weaponsTypes(ID)
        );)";

        sql += R"(
        CREATE TABLE IF NOT EXISTS heroStats (
        HERO_ID	INTEGER NOT NULL UNIQUE,
        NAME	TEXT NOT NULL,
        KILLS	INTEGER NOT NULL DEFAULT 0,
        XP_GAINED_FROM_KILLS	INTEGER NOT NULL DEFAULT 0,
        PRIMARY KEY(HERO_ID)
        );)";

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
    const char* insertSQL = "INSERT INTO heroes (HERO, XP, LEVEL, COINS, HP, STRENGTH, WEAPON_TYPE_ID, WEAPON_HITS) VALUES (?, 0, 1, 0, ?, ?, 0, 0);";

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


    return lastInsertID; //Player UID
}

bool sqlDB::searchForHeroes(std::vector<character> &buffer) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;

    const char* selectSQL = "\
                            SELECT heroes.ID, heroes.HERO, heroes.XP, heroes.LEVEL, heroes.COINS, heroes.HP, heroes.STRENGTH, heroes.WEAPON_TYPE_ID, heroes.WEAPON_HITS, \
                                weaponsTypes.DURABILITY, weaponsTypes.MODIFIER, weaponsTypes.PRICE, \
                            CASE \
                                WHEN weaponsTypes.NAME IS NULL THEN \"\" \
                                ELSE weaponsTypes.NAME \
                            END AS NAME \
                            FROM heroes \
                            LEFT JOIN weaponsTypes ON weaponsTypes.ID = WEAPON_TYPE_ID;\
        ";//Komma efter PRICE er meningen

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(mSqlDB, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare SQL statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    // Clear the buffer
    buffer.clear();

    // Iterate through the results of the query
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve and assign values from the database row
        int ID = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));         // HERO
        int xp = sqlite3_column_int(stmt, 2);                                                   // XP
        int level = sqlite3_column_int(stmt, 3);                                                // LEVEL
        int coins = sqlite3_column_int(stmt, 4);                                                // COINS
        int hp = sqlite3_column_int(stmt, 5);                                                   // HP
        int strength = sqlite3_column_int(stmt, 6);                                             // STRENGTH

        int weaponID = sqlite3_column_int(stmt, 7);                                             // WEAPON ID
        int weaponHits = sqlite3_column_int(stmt, 8);                                           // WEAPON HITS
        int weaponDurability = sqlite3_column_int(stmt, 9);                                     // WEAPON DURABILITY
        int modifier = sqlite3_column_int(stmt, 10);                                             // WEAPON MODIFIER
        int weaponPrice = sqlite3_column_int(stmt, 11);                                         // WEAPON PRICE
        std::string weaponName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));  // WEAPON NAME

        character hero(name, ID, xp, level, coins, hp, strength);
        if(weaponID != 0) {
            Weapon armory(weaponName, weaponDurability, weaponHits, modifier, weaponPrice);
            hero.setWeapon(armory);
            //std::cout << "wName: " << weaponName << ", Durability: " << weaponDurability << ", Hits: " << weaponHits << ", Mod: " << modifier << ", Price: " << weaponPrice << std::endl;
        }


        // Add the hero to the buffer
        buffer.push_back(hero);
    }

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);

    std::cout << "[INFO]: Successfully retrieved heroes from the database." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //DEBUG
    return true;
}

bool sqlDB::saveHero(const character &hero) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = "UPDATE heroes SET `XP` = ?, `LEVEL` = ?, `COINS` = ?, `HP` = ?, `STRENGTH` = ?, `WEAPON_TYPE_ID` = COALESCE(\
        (SELECT ID FROM weaponsTypes WHERE NAME = ? LIMIT 1), \
        0 ), `WEAPON_HITS` = ? WHERE `ID` = ?;";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    //Hero
    sqlite3_bind_int(stmt, 1, hero.getXP());
    sqlite3_bind_int(stmt, 2, hero.getLvl());
    sqlite3_bind_int(stmt, 3, hero.getCoins());
    sqlite3_bind_int(stmt, 4, hero.getHp());
    sqlite3_bind_int(stmt, 5, hero.getStrength());


    //Hero's weapon
    int getHits = 0;
    std::string getName {};
    if(hero.getWeapon() != nullptr) {
        getHits = hero.getWeapon()->getRemainingHits();
        getName = hero.getWeapon()->getName();
    }
    sqlite3_bind_text(stmt, 6, getName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, getHits);
    sqlite3_bind_int(stmt, 8, hero.getId());

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

bool sqlDB::loadWeaponShop(const character &hero, std::vector<Weapon> &shopItems) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    const char* mSQLquery = R"(
                            SELECT ID, NAME, MODIFIER, DURABILITY, PRICE
                                FROM weaponsTypes
                                WHERE PRICE < ?
                                ORDER BY PRICE DESC
                                LIMIT 10
                            )";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(mSqlDB, mSQLquery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare SQL statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, hero.getCoins());

    // Clear the buffer
    shopItems.clear();

    // Iterate through the results of the query
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve and assign values from the database row
        int ID = sqlite3_column_int(stmt, 0);                                                   //WEAPON ID
        std::string weaponName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));   // WEAPON NAME
        int modifier = sqlite3_column_int(stmt, 2);                                            // WEAPON MODIFIER
        int weaponDurability = sqlite3_column_int(stmt, 3);                                     // WEAPON DURABILITY
        int weaponPrice = sqlite3_column_int(stmt, 4);                                         // WEAPON PRICE

        Weapon armory(weaponName, weaponDurability, 0, modifier, weaponPrice);
        // Add the weapon to the buffer
        shopItems.push_back(armory);
    }

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);

    std::cout << "[INFO]: Successfully retrieved weapons from the database." << std::endl;

    return true;
}

bool sqlDB::addWeaponType(std::string name, int durability, int modifier, int price) {

    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = "INSERT INTO weaponsTypes (NAME, DURABILITY, MODIFIER, PRICE) SELECT ?, ?, ?, ? WHERE NOT EXISTS ( SELECT 1 FROM weaponsTypes WHERE NAME = ? );";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, durability);
    sqlite3_bind_int(stmt, 3, modifier);
    sqlite3_bind_int(stmt, 4, price);
    sqlite3_bind_text(stmt, 5, name.c_str(), -1, SQLITE_STATIC);


    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR]: Failed to execute statement: \n" << sqlite3_errmsg(mSqlDB) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    // Get the last inserted row ID
    int lastInsertID = static_cast<int>(sqlite3_last_insert_rowid(mSqlDB));

    if(lastInsertID != 0) {
        std::cout << "[INFO]: You found a new weapon called " << name << std::endl;
    }
    else {
        std::cout << "[INFO]: The weapon you took in the cave\nis already discovered: " << name << std::endl;
    }



    // Finalize the statement to free resources
    sqlite3_finalize(stmt);


    //return lastInsertID;

    return true;
}


bool sqlDB::addToStats(const character &hero, int kills, int XP) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = R"(
                                INSERT INTO herostats (HERO_ID, NAME, KILLS, XP_GAINED_FROM_KILLS) VALUES (?, ?, ?, ?)
                                ON CONFLICT(HERO_ID) DO UPDATE
                                SET KILLS = KILLS + excluded.KILLS, XP_GAINED_FROM_KILLS = XP_GAINED_FROM_KILLS + excluded.XP_GAINED_FROM_KILLS;
                            )";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return -1;
    }

    sqlite3_bind_int(stmt, 1, hero.getId());
    sqlite3_bind_text(stmt, 2, hero.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, kills);
    sqlite3_bind_int(stmt, 4, XP);


    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR]: Failed to execute statement: \n" << sqlite3_errmsg(mSqlDB) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }
    // Finalize the statement to free resources
    sqlite3_finalize(stmt);

    return true;
}

bool sqlDB::loadStats(std::vector<statEntry> &stats) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    const char* mSQLquery = R"(
                            SELECT HERO_ID, NAME, KILLS, XP_GAINED_FROM_KILLS
                                FROM heroStats
                                WHERE 1
                            )";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(mSqlDB, mSQLquery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare SQL statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    std::vector<statEntry> temp {};

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        statEntry line;

        line.id = sqlite3_column_int(stmt, 0);                                                  //HERO ID
        line.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));                // NAME
        line.kills = sqlite3_column_int(stmt, 2);                                               // KILLS
        line.xpFromKills = sqlite3_column_int(stmt, 3);                                         // XP_GAINED_FROM_KILLS

        temp.push_back(line);
    }

    stats = temp;

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);

    std::cout << "[INFO]: Successfully retrieved stats from the database." << std::endl;

    return true;

}
