////////////////////////////////////////////////////////////////////////
///
///     This game is created as the assignment submission
///     for the course exam in Software development, spring 2025
///     on SDU Odense
///
///     Made by Frederik Vester
///
////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <iomanip>
//#include <cstdlib> // For system()
#include <unistd.h>
#include <chrono>
#include <thread>
#include <string>
#include <random>
#include <vector>

#include "monster.h"
#include "character.h"
#include "SQLdatabase.h"
#include "keyboard.h"
#include "world.h"
#include "caves.h"

// Game settings
#define WIDTH 40
#define HEIGHT 19
#define PADDLE_SIZE 3
#define FRAME_RATE 3 //Fjerner også fejlmeddelelser hurtigere
#define MAX_NAME_LENGTH 10

#define DRAGON_NAME "DRAGON BOSS"
#define XP_GAIN_FROM_COIN_MULTIPLIED_BY_LEVEL 100

//Keys
#define KEY_LEFT 97 // 'a'
#define KEY_RIGHT 100 // 'd'
#define KEY_UP 119 // 'w'
#define KEY_DOWN 115 // 's'
#define KEY_ENTER 10 // ENTER KEY
#define KEY_QUIT 113 // 'q'
#define KEY_ESC 27 // ESC KEY
#define KEY_BACKSPACE 127 //BACKSPACE KEY

#define TerminalClear() std::cout << "\x1B[2J\x1B[H" //std::system("clear"); før men den tager meget computerkraft

enum gameState {STARTMENU, GAME, CREATE_PLAYER, LOAD_PLAYER, ABOUT, MONSTER, MONSTER_FIGHT, MONSTER_STATUS, CAVE_INTRO, CAVE, GAME_OVER, WON, EXIT};
enum gameState stateOfGame;

//Variabler
static bool enterPressed;
static int menuPos = 3; //standard
static int menuOptions = 4; //Max

static std::string tmpName = {};
//static std::string windowArr[HEIGHT] = {};
std::vector<character> loadingPlayers {};

static std::string msgField {};
static std::chrono::time_point<std::chrono::steady_clock> timeOut;

keyboard keyboardCTRL;
static bool killKeyboardThread = false;

std::default_random_engine generator;

//World
World verden(WIDTH, HEIGHT);

//SQL
sqlDB DB("saves.db");

void drawGame(character &player, monster &pMonster, cave &currentCave) {

    std::cout << "[ XP: " << std::setw(6) << std::right << player.getXP() << " Lvl: " << std::setw(2) << player.getLvl() << std::right << " NAME: " << std::setw(12) << std::right << player.getName() << " ]" << std::endl;
    std::cout << "[ pID: " << std::setw(5) << player.getId() << " pHP: " << std::setw(5) << player.getHp() << " pDMG: " << std::setw(5) << player.getStrength() << std::setw(WIDTH-33) << " ]" /*<< player.getPlayerPos() */<< std::endl;
    std::cout << "[ Coins: " << std::setw(5) << player.getCoins() << std::setw(WIDTH - 12) << " ]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;

    for(int y = 0; y <= HEIGHT; y++) {
        switch(y) {
        case 0:
        {
            char taken {};
            if(verden.getLine(2).length() >= player.getPlayerPos()) {
                taken = verden.getLine(2).at(player.getPlayerPos()-1); //Det her er den første process der kører. Ergo er den vi skal kigge efter 2 oppe når vi kigger efter den
            }
            if(taken == 'C') {
                int xp = player.getXP();
                player.setXP(xp+(player.getLvl()*XP_GAIN_FROM_COIN_MULTIPLIED_BY_LEVEL));
                if(player.getHp() < (10+(2*player.getLvl()))) {
                    player.gainHP();
                }
                if(player.getStrength() < (2+player.getLvl())) {
                    player.gainStrength();
                }
                player.addCoins(1);
                std::cout << '\a';
            }
            if(taken == 'M') {
                std::uniform_int_distribution<int> distribution(MIN_MONSTER_LEVEL, MAX_MONSTER_LEVEL);
                int randomLevel = distribution(generator);
                pMonster = monster(randomLevel);
                std::cout << '\a';
                menuPos = 0;
                stateOfGame = MONSTER;
            }
            if(taken == 'D') {
                pMonster = monster(MAX_MONSTER_LEVEL, DRAGON_NAME);
                pMonster.setDragon(true);
                std::cout << '\a';
                stateOfGame = MONSTER_FIGHT;
                menuPos = 0;
            }
            if(taken == '#') { //Cave
                stateOfGame = CAVE_INTRO;
                currentCave = cave(player.getLvl());
                std::cout << '\a'; //Alert sound
                menuPos = 0;
            }
            break;
        }
        case HEIGHT:
            verden.newLine();
            break;
        default:
            verden.moveLines(y);
        }
    }

    for(int y = HEIGHT; y >= 0; --y) {

        switch(y) {
        case 0:
            std::cout << "[" << std::setw(player.getPlayerPos()) << "H" << std::setw(WIDTH - player.getPlayerPos()+1) << "]" << std::endl;
            break;
        default:
            std::cout << "[" << verden.getLine(y) << "]" << std::endl;
        }


    }


    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }

}

void drawCaveConflict( character& player,  cave& caveCnf ) {


    std::string playerStats = "You have " + std::to_string(player.getHp())  + " HP and " + std::to_string(player.getStrength()) + " damage!";

    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-6) << "You have entered a cave level: " << std::setw(4) << caveCnf.getCaveLevel() << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "If you clear the cave, you can" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "earn gold - so the nearby villagers" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "can use it as a mine" << " ]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "The cave contains:" << " ]" << std::endl;
    for(monster newMonster:caveCnf.getMonsters()) {
        std::string monsterNameString = newMonster.getName() + " - " + std::to_string(newMonster.getHp()) + " HP + " + std::to_string(newMonster.getDamage()) + " dmg!";
        std::cout << "[ " << std::setw(WIDTH-2) << monsterNameString << " ]" << std::endl;
    }

    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << playerStats << " ]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Do you dare to fight?" << "]" << std::endl;

    switch(menuPos) {
    case 0:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "->FIGHT<-" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-20) << std::right << "FLEE" << std::setw(18) << "" << " ]" << std::left << std::endl;
        break;
    case 1:
        std::cout << "[ " << std::setw(WIDTH-19) << std::right << "FIGHT" << std::setw(17) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-18) << std::right << "->FLEE<-" << std::setw(16) << "" << " ]" << std::left << std::endl;
        break;
    default:
        std::cout << "[ " << std::setw(WIDTH-1) << "Out of bounds for some reason" << " ]" << std::endl;
    }

    if(enterPressed) {
        enterPressed = false;

        if(menuPos == 0) {
            stateOfGame = CAVE;
            menuPos = 0;
        } else {
            stateOfGame = GAME;
        }
    }

    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }
}

void drawCaveFight( character& player,  cave& caveCnf ) {



    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "FIGHT!!" << " ]" << std::endl;

    int i = 0;
    for(monster &newMonster:caveCnf.getMonsters()) {
        ++i;
        if(newMonster.getHp() > 0) {
            std::string monsterCountStr = std::to_string(i) + "/" + std::to_string(caveCnf.getMonsters().size()) ;
            std::cout << "[ " << std::setw(WIDTH-2) << std::right << monsterCountStr << " ]" << std::left << std::endl;


            std::string monsterNameString = "You have met " + newMonster.getName() + "(" + std::to_string(newMonster.getLvl()) + ")";
            std::string monsterStats = "It has " + std::to_string(newMonster.getHp())  + " HP and " + std::to_string(newMonster.getDamage()) + " damage!";
            std::string playerStats = "You have " + std::to_string(player.getHp())  + " HP and " + std::to_string(player.getStrength()) + " damage!";


            std::cout << "[ " << std::setw(WIDTH-2) << monsterNameString << " ]" << std::endl;
            std::cout << "[ " << std::setw(WIDTH-2) << monsterStats << " ]" << std::endl;
            std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
            std::cout << "[ " << std::setw(WIDTH-2) << playerStats << " ]" << std::endl;
            std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

            std::uniform_int_distribution<int> distribution(25,100);
            double hitSuccess = (static_cast<double>(distribution(generator))/100);

            std::string successString = std::to_string(static_cast<int>(hitSuccess*100)) + "%";
            std::string damageStr;

            std::cout << "[ " << std::setw(WIDTH/2-1) << "Hitsuccess: " << std::setw(WIDTH/2-1) << successString << " ]" << std::endl;


            switch(menuPos) {
            case 0:
                damageStr = "Your turn: " + std::to_string(player.getStrength() * hitSuccess)+" damage!!";
                std::cout << "[ " << std::setw(WIDTH-1) << std::left << damageStr << "]" << std::endl;
                newMonster.hit(player.getStrength() * hitSuccess);
                menuPos = 1;
                break;
            case 1:
                damageStr = "Opponents turn: " + std::to_string(newMonster.getDamage() * hitSuccess)+" damage!!";
                std::cout << "[ " << std::setw(WIDTH-1) << std::left << damageStr << "]" << std::endl;
                player.hit(newMonster.getDamage() * hitSuccess);
                menuPos = 0;
                break;
            default:
                std::cout << "[ " << std::setw(WIDTH-1) << "Out of bounds for some reason" << " ]" << std::endl;
            }

            if(player.getHp() <= 0) {
                if(!DB.killHero(player)) { //Hvis SQL fejler. Så man kan se fejlen
                    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
                }
                player = character();

                stateOfGame = GAME_OVER;
                return;
            }

            break;
        }

    }




    if( caveCnf.getMonsters().at(
                caveCnf.getMonsters().size()-1
            ).getHp() <= 0 ) { //All monsters are dead. Har bare splittet den op så det er mere læsbart

        int xp = player.getXP();
        int winGold = caveCnf.returnGold();

        for( monster &newMonster:caveCnf.getMonsters() ) {
            xp += newMonster.getWinXP();
        }

        player.setXP(xp);

        stateOfGame = GAME;

    }




    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(750)); //Så det ikke går alt for hurtigt
}

void drawMonsterConflict( character& player,  monster& monsterKiller ) {

    std::string monsterNameString = "You have met " + monsterKiller.getName();
    std::string monsterStats = "It has " + std::to_string(monsterKiller.getHp())  + " HP and " + std::to_string(monsterKiller.getDamage()) + " damage!";
    std::string playerStats = "You have " + std::to_string(player.getHp())  + " HP and " + std::to_string(player.getStrength()) + " damage!";

    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << monsterNameString << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << monsterStats << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << playerStats << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Do you dare to fight?" << "]" << std::endl;

    switch(menuPos) {
    case 0:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "->FIGHT<-" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-20) << std::right << "FLEE" << std::setw(18) << "" << " ]" << std::left << std::endl;
        break;
    case 1:
        std::cout << "[ " << std::setw(WIDTH-19) << std::right << "FIGHT" << std::setw(17) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-18) << std::right << "->FLEE<-" << std::setw(16) << "" << " ]" << std::left << std::endl;
        break;
    default:
        std::cout << "[ " << std::setw(WIDTH-1) << "Out of bounds for some reason" << " ]" << std::endl;
    }

    if(enterPressed) {
        enterPressed = false;

        if(menuPos == 0) {
            stateOfGame = MONSTER_FIGHT;
        } else {
            stateOfGame = GAME;
        }
    }

    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }
}

void drawMonsterFight( character& player,  monster& monsterKiller ) {

    std::string monsterNameString = "You have met " + monsterKiller.getName();
    std::string monsterStats = "It has " + std::to_string(monsterKiller.getHp())  + " HP and " + std::to_string(monsterKiller.getDamage()) + " damage!";
    std::string playerStats = "You have " + std::to_string(player.getHp())  + " HP and " + std::to_string(player.getStrength()) + " damage!";

    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << monsterNameString << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << monsterStats << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << playerStats << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    std::uniform_int_distribution<int> distribution(25,100);
    double hitSuccess = (static_cast<double>(distribution(generator))/100);

    std::string successString = std::to_string(static_cast<int>(hitSuccess*100)) + "%";
    std::string damageStr;

    std::cout << "[ " << std::setw(WIDTH/2-1) << "Hitsuccess: " << std::setw(WIDTH/2-1) << successString << " ]" << std::endl;


    switch(menuPos) {
    case 0:
        damageStr = "Your turn: " + std::to_string(player.getStrength() * hitSuccess)+" damage!!";
        std::cout << "[ " << std::setw(WIDTH-1) << std::left << damageStr << "]" << std::endl;
        monsterKiller.hit(player.getStrength() * hitSuccess);
        menuPos = 1;
        break;
    case 1:
        damageStr = "Opponents turn: " + std::to_string(monsterKiller.getDamage() * hitSuccess)+" damage!!";
        std::cout << "[ " << std::setw(WIDTH-1) << std::left << damageStr << "]" << std::endl;
        player.hit(monsterKiller.getDamage() * hitSuccess);
        menuPos = 0;
        break;
    default:
        std::cout << "[ " << std::setw(WIDTH-1) << "Out of bounds for some reason" << " ]" << std::endl;
    }

    if(monsterKiller.getHp() <= 0) {
        int xp = player.getXP();
        player.setXP(xp+monsterKiller.getWinXP());

        if(monsterKiller.isDragon()) {
            stateOfGame = WON;
        }
        else {
            stateOfGame = GAME;
        }

    }

    if(player.getHp() <= 0) {
        if(!DB.killHero(player)) { //Hvis SQL fejler. Så man kan se fejlen
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
        player = character();

        stateOfGame = GAME_OVER;
        return;
    }


    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000)); //Så det ikke går alt for hurtigt
}

void drawGameOver() {

    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "You died and thereby" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "lost the game!" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Start a new game and try again" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "This character is lost forever" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Press ESC/ENTER to continue" << "]" << std::endl;

}

void drawGameWon() {

    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "You WON!!!" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "against all odds" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Start a new game and try again" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "or continue on this character" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Press ESC/ENTER to continue" << "]" << std::endl;

}



void drawMenu() {
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    std::cout << "[ " << std::setw(WIDTH-1) << "Controls:" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "LEFT: A, RIGHT: D" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "UP: W, DOWN: S" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "ENTER: ENTER, QUIT: ESC" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "AUTOSAVE ON ESC" << " ]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    switch(menuPos) {
    case 3:
        std::cout << "[ " << std::setw(WIDTH-15) << std::right << "->LOAD GAME<-" << std::setw(13) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "NEW GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "ABOUT" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "EXIT GAME" << std::setw(15) << "" << " ]" << std::endl;
        break;
    case 2:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "LOAD GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-15) << "->NEW GAME<-" << std::setw(13) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "ABOUT" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "EXIT GAME" << std::setw(15) << "" << " ]" << std::endl;
        break;
    case 1:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "LOAD GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "NEW GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-15) << "->ABOUT<-" << std::setw(13) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "EXIT GAME" << std::setw(15) << "" << " ]" << std::endl;
        break;
    case 0:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "LOAD GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "NEW GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "ABOUT" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-15) << "->EXIT GAME<-" << std::setw(13) << "" << " ]" << std::endl;
        break;
    default:
        std::cout << "[ " << std::setw(WIDTH) << std::right << "[DEBUG]: Menu: " << menuPos << "" << " ]" << std::endl;
        ;
    }

    if(enterPressed) {
        enterPressed = false;
        switch(menuPos) {
        case 3:
            DB.searchForHeroes(loadingPlayers);
            stateOfGame = LOAD_PLAYER;
            menuPos = 0;
            break;
        case 2:
            tmpName =""; //Her kører den kun 1 gang
            stateOfGame = CREATE_PLAYER;
            break;
        case 1:
            stateOfGame = ABOUT;
            break;
        case 0:
            stateOfGame = EXIT;
            break;
        default:
            std::cout << "[ " << std::setw(WIDTH) << std::right << "[DEBUG]: EnterMenu: " << menuPos << "" << " ]" << std::endl;
            ;
        }
    }


    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }
}

void drawPlayerCreationMenu( character& player ) {
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    std::cout << "[ " << std::setw(WIDTH-1) << "Write your players name:" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "(max 10 letters)" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << tmpName << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "and press ENTER" << " ]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    if(enterPressed) {
        enterPressed = false;

        int sqlResult = DB.addNewHero(tmpName); //Hvis den ikke er lig med -1 så har vi success
        if(sqlResult != -1) {

            player = character(tmpName, sqlResult);
            player.setPlayerPos(WIDTH/2);

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            verden.clearField();

            stateOfGame = GAME;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // for errordetection
            stateOfGame = STARTMENU;
        }

    }

    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }
}

void drawLoadPlayerMenu( character& player ) {

    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Pick a character:" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    for(int i = 0; i < loadingPlayers.size(); ++i) {
        character hero = loadingPlayers[i];

        if(i == menuPos) {
            std::string input = "->"+ hero.getName() + "; lvl: " + std::to_string(hero.getLvl()) + "<-";
            std::cout << "[ " << std::setw(WIDTH-15) << std::right << input << std::setw(13) << "" << " ]" << std::endl;
        } else {
            std::string input = hero.getName() + "; lvl: " + std::to_string(hero.getLvl());
            std::cout << "[ " << std::setw(WIDTH-17) << std::right << input << std::setw(15) << "" << " ]" << std::endl;
        }
    }

    if(enterPressed) {
        enterPressed = false;

        player = loadingPlayers[menuPos];
        player.setPlayerPos(WIDTH/2);

        verden.clearField();

        stateOfGame = GAME;
    }


    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }
}

void drawAbout() {
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    std::cout << "[ " << std::setw(WIDTH-1) << "You are a hero. - You are marked 'H'" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Kill all the monsters you can to gain" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "XP, but avoid them you cannot." << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << " - Monsters are marked 'M'" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Most monsters you can flee from" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "But the dragon you cannot" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "The dragon is the hardest to defeat." << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "You need to defeat the dragon to win." << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << " - The dragon is marked 'D'" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "Gain as much experience as you can" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "and try your luck" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1) << "The games saves automatically on" << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH-20) << std::right << "STARTMENU"<< std::setw(WIDTH-19) << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-1)<< std::left << "Do not die or you will have to start " << "]" << std::endl;
    std::cout << "[" << std::setw(WIDTH-20) << std::right << "OVER!!!"  << std::setw(WIDTH-19) << "]" << std::endl;

    if(timeOut > std::chrono::steady_clock::now()) {
        std::cout << "[----------------------------------------]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-2) << msgField << " ]" << std::endl;
        std::cout << "[----------------------------------------]" << std::endl;
    }
}

void keyboardCTRLFunc( character &player ) {

    int bogstav;
    do
    {
        if(killKeyboardThread == true) {
            goto exitLoop_1;
        }
        if(keyboardCTRL.kbhit()) {
            bogstav=keyboardCTRL.getch();
            //std::cout << "[DEBUG]: gameState: " << stateOfGame << std::endl;
            if(stateOfGame == STARTMENU) {
                switch(bogstav) {
                case KEY_UP:
                    if(menuPos < (menuOptions-1)) { ++menuPos; }
                    break;
                case KEY_DOWN:
                    if(menuPos > 0) { --menuPos; }
                    break;
                case KEY_ESC:

                    if( player.getId() != -1 ) {
                        stateOfGame = GAME;
                        //bogstav = 0; //DEBUG
                    }
                    break;
                case KEY_ENTER:
                    enterPressed = true;
                    break;

                }

            }
            else if(stateOfGame == GAME) {
                switch(bogstav) {
                case KEY_LEFT:
                    if(player.getPlayerPos() > 1) { player.movePlayer(-1); }
                    break;
                case KEY_RIGHT:
                    if(player.getPlayerPos() < WIDTH) { player.movePlayer(1); }
                    break;
                case KEY_QUIT:
                case KEY_ESC:
                    menuPos = 3; //Top af menu
                    stateOfGame = STARTMENU;
                    if(DB.saveHero(player)) {
                        timeOut = std::chrono::steady_clock::now() + std::chrono::seconds(5);
                        msgField = "GAME SAVED";
                        //std::cout << "GAME SAVED" << std::endl;
                    } else {
                        timeOut = std::chrono::steady_clock::now() + std::chrono::seconds(5);
                        msgField = "GAME NOT SAVED - CHECK SQL";
                        //std::cout << "GAME NOT SAVED - CHECK SQL" << std::endl;
                    }
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    //std::cout << "[DEBUG]: KEY_ESC pressed" << std::endl;
                    //goto exitLoop_1;
                    break;

                default:
                    std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;
                    ;
                }
            }
            else if(stateOfGame == MONSTER) {
                switch(bogstav) {
                case KEY_UP:
                    if(menuPos > 0) { --menuPos; }
                    break;
                case KEY_DOWN:
                    if(menuPos < (menuOptions-1)) { ++menuPos; }
                    break;
                case KEY_ESC:

                    if( player.getId() != -1 ) {
                        stateOfGame = GAME;
                        bogstav = 0; //DEBUG
                    }
                    break;
                case KEY_ENTER:
                    enterPressed = true;
                    break;

                }
            }
            else if(stateOfGame == CAVE_INTRO) {
                switch(bogstav) {
                case KEY_UP:
                    if(menuPos > 0) { --menuPos; }
                    break;
                case KEY_DOWN:
                    if(menuPos < (menuOptions-1)) { ++menuPos; }
                    break;
                case KEY_ESC:

                    if( player.getId() != -1 ) {
                        stateOfGame = GAME;
                        bogstav = 0; //DEBUG
                    }
                    break;
                case KEY_ENTER:
                    enterPressed = true;
                    break;

                }
            }
            else if(stateOfGame == CREATE_PLAYER) {
                if(tmpName.length() <= MAX_NAME_LENGTH) {
                    //std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;
                    switch(bogstav) {
                    case 'a':
                    case 'A':
                        tmpName += "A";
                        break;
                    case 'b':
                    case 'B':
                        tmpName += 'B';
                        break;
                    case 'c':
                    case 'C':
                        tmpName += 'C';
                        break;
                    case 'd':
                    case 'D':
                        tmpName += 'D';
                        break;
                    case 'e':
                    case 'E':
                        tmpName += 'E';
                        break;
                    case 'f':
                    case 'F':
                        tmpName += 'F';
                        break;
                    case 'g':
                    case 'G':
                        tmpName += 'G';
                        break;
                    case 'h':
                    case 'H':
                        tmpName += 'H';
                        break;
                    case 'i':
                    case 'I':
                        tmpName += 'I';
                        break;
                    case 'j':
                    case 'J':
                        tmpName += 'J';
                        break;
                    case 'k':
                    case 'K':
                        tmpName += 'K';
                        break;
                    case 'l':
                    case 'L':
                        tmpName += 'L';
                        break;
                    case 'm':
                    case 'M':
                        tmpName += 'M';
                        break;
                    case 'n':
                    case 'N':
                        tmpName += 'N';
                        break;
                    case 'o':
                    case 'O':
                        tmpName += 'O';
                        break;
                    case 'p':
                    case 'P':
                        tmpName += 'P';
                        break;
                    case 'q':
                    case 'Q':
                        tmpName += 'Q';
                        break;
                    case 'r':
                    case 'R':
                        tmpName += 'R';
                        break;
                    case 's':
                    case 'S':
                        tmpName += 'S';
                        break;
                    case 't':
                    case 'T':
                        tmpName += 'T';
                        break;
                    case 'u':
                    case 'U':
                        tmpName += 'U';
                        break;
                    case 'v':
                    case 'V':
                        tmpName += 'V';
                        break;
                    case 'w':
                    case 'W':
                        tmpName += 'W';
                        break;
                    case 'x':
                    case 'X':
                        tmpName += 'X';
                        break;
                    case 'y':
                    case 'Y':
                        tmpName += 'Y';
                        break;
                    case 'z':
                    case 'Z':
                        tmpName += 'Z';
                        break;
                    case KEY_BACKSPACE:
                        {
                            if(tmpName.length() != 0) {
                                tmpName.erase(tmpName.length()-1, 1);
                            }
                            break;
                        }
                    case KEY_ENTER:
                        enterPressed = true;
                        break;
                    case KEY_ESC:
                        menuPos = 3;
                        stateOfGame = STARTMENU;
                        std::cout << "[DEBUG]: KEY_ESC pressed" << std::endl;
                        //goto exitLoop_1;
                        break;

                    default:
                        std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;
                        ;
                    }
                } else {
                    switch(bogstav) {
                    case KEY_ENTER:
                        enterPressed = true;
                        break;
                    case KEY_ESC:
                        stateOfGame = STARTMENU;
                    }
                }
            }
            else if(stateOfGame == LOAD_PLAYER) {
                switch(bogstav) {
                case KEY_UP:
                    if(menuPos > 0) { --menuPos; }
                    break;
                case KEY_DOWN:
                    if(loadingPlayers.size() >= 1) {
                        if(menuPos < (loadingPlayers.size()-1)) { ++menuPos; }
                    }
                    break;
                case KEY_ESC:
                    menuPos = 3;
                    stateOfGame = STARTMENU;
                    break;
                case KEY_ENTER:
                    enterPressed = true;
                    break;

                }
            }
            else if(stateOfGame == ABOUT) {
                switch(bogstav) {
                case KEY_ENTER:
                case KEY_QUIT:
                case KEY_ESC:
                    stateOfGame = STARTMENU;
                    //goto exitLoop_1;
                    break;
                }
            }
            else if(stateOfGame == GAME_OVER) {
                switch(bogstav) {
                case KEY_ENTER:
                case KEY_QUIT:
                case KEY_ESC:
                    stateOfGame = STARTMENU;
                    std::cout << "[DEBUG]: KEY_ESC pressed" << std::endl;
                    //goto exitLoop_1;
                    break;

                default:
                    std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;
                    ;
                }
            }
            else if(stateOfGame == WON) {
                switch(bogstav) {
                case KEY_QUIT:
                case KEY_ESC:
                    stateOfGame = STARTMENU;
                    std::cout << "[DEBUG]: KEY_ESC pressed" << std::endl;
                    //goto exitLoop_1;
                    break;
                case KEY_ENTER:
                    stateOfGame = GAME;
                default:
                    std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;
                    ;
                }
            }

            fflush(stdin);
            tcflush(0, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while(1);
    exitLoop_1:
    std::cout << "Exit of keyboard thread" << std::endl;
}

int main()
{
    //Setup
    TerminalClear(); //cls for windows
    std::cout << "KILL THE DRAGON... booting up" << std::endl;


    srand(time(0));
    stateOfGame = STARTMENU;

    verden.clearField();

    if(!DB.isOpen()) {
        std::cout << "[ERROR]: Database did not open" << std::endl;
        return 1; //Ikke nul, men alt andet da 0 er når den afslutter med success
    }

    if(!DB.checkUserTable(MAX_NAME_LENGTH)) {
        std::cout << "[ERROR]: Table not okay" << std::endl;
        return 1; //Ikke nul, men alt andet da 0 er når den afslutter med success
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    character player;
    monster currentMonster;
    cave currentCave;


    std::thread keyboardThread(keyboardCTRLFunc, std::ref(player));

    do
    {
        TerminalClear(); //cls for windows
        if(player.getXP() >= (player.getLvl()*1000)) {
            player.lvlUp();
        }

        switch(stateOfGame) {
        case STARTMENU:
            menuOptions = 4;
            drawMenu();
            break;
        case LOAD_PLAYER:
            menuOptions = loadingPlayers.size();
            drawLoadPlayerMenu(player);
            break;
        case CREATE_PLAYER:
            drawPlayerCreationMenu(player);
            break;
        case ABOUT:
            drawAbout();
            break;
        case GAME:
            drawGame(player, currentMonster, currentCave);
            break;
        case MONSTER:
            menuOptions = 2;
            drawMonsterConflict(player, currentMonster);
            break;
        case MONSTER_FIGHT:
            menuOptions = 2; //Jeg bruger den selvom det ikke er noget man selv kan styre i spillet
            drawMonsterFight(player, currentMonster);
            break;
        case CAVE_INTRO:
            menuOptions = 2;
            drawCaveConflict(player, currentCave);
            break;
        case CAVE:
            menuOptions = 2;//Jeg bruger den selvom det ikke er noget man selv kan styre i spillet
            drawCaveFight(player, currentCave);
            break;
        case GAME_OVER:
            drawGameOver();
            break;
        case WON:
            drawGameWon();
            break;
        case EXIT:
            goto leaveGame;
            break;
        default:
            std::cout << "[DEBUG]: ERROR.. NO MENU !" << std::endl;
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FRAME_RATE));
    }
    while(1);
leaveGame:
    std::cout << "Thank you for playing!" << std::endl;
    while(1) {
        killKeyboardThread = true;
        if(keyboardThread.joinable()) {
            keyboardThread.join();
            break;
        }
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}
