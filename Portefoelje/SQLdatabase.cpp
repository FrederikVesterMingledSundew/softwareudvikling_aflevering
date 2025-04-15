#include "SQLdatabase.h"
#include <iostream>
#include <cstring>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

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

bool sqlDB::checkUserTable(const int heroNameLength) {
    char *zErrMsg = 0;
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }
    std::string sql = "CREATE TABLE heroes(\
        ID INTEGER PRIMARY KEY AUTOINCREMENT,\
        HERO TEXT NOT NULL,\
        XP INTEGER NOT NULL,\
        LEVEL INTEGER NOT NULL,\
        HP INTEGER NOT NULL,\
        STRENGTH INTEGER NOT NULL );";
    /* Execute SQL statement */
    this->rc = sqlite3_exec(mSqlDB, sql.c_str(), callback, 0, &zErrMsg);

    if( this->rc != SQLITE_OK ){
        //Table already exists
        if(!strcmp(zErrMsg, "table heroes already exists")) {
            return true;
        } else {
            //Table error
            fprintf(stderr, "SQL error: %s(%i)\n", zErrMsg, this->rc);
            sqlite3_free(zErrMsg);
            return false;
        }
    } else {
        return true; //Table created successfully
    }
    return 0;

}

int sqlDB::addNewHero(std::string &name) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return -1;
    }

    sqlite3_stmt* stmt;
    const char* insertSQL = "INSERT INTO heroes (HERO, XP, LEVEL, HP, STRENGTH) VALUES (?, 0, 0, ?, ?);";

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
    return true;
}

bool sqlDB::searchForHeroes(std::vector<character> &buffer) {
    if(!this->isOpen()) {
        std::cout << "[ERROR]: Database is not open." << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    const char* selectSQL = "SELECT `ID`, `HERO`, `XP`, `LEVEL`, `HP`, `STRENGTH` FROM heroes";

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
        int hp = sqlite3_column_int(stmt, 4);                                    // HP
        int strength = sqlite3_column_int(stmt, 5);                              // STRENGTH
        character hero(name, ID, xp, level, hp, strength);

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

    /*sqlite3_stmt* stmt;
    const char* insertSQL = "INSERT INTO heroes (HERO, XP, LEVEL, HP, STRENGTH) VALUES (?, 0, 0, ?, ?);";

    if (sqlite3_prepare_v2(mSqlDB, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR]: Failed to prepare statement: " << sqlite3_errmsg(mSqlDB) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, START_HP);
    sqlite3_bind_int(stmt, 3, START_STRENGTH);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR]: Failed to execute statement: \n" << sqlite3_errmsg(mSqlDB) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }


    // Finalize the statement to free resources
    sqlite3_finalize(stmt);*/
    return true;
}
