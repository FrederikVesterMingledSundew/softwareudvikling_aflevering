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
#include <cstdlib> // For system()
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

// Game settings
#define WIDTH 40
#define HEIGHT 19
#define PADDLE_SIZE 3
#define FRAME_RATE 2
#define MAX_NAME_LENGTH 10

//Keys
#define KEY_LEFT 97 // 'a'
#define KEY_RIGHT 100 // 'd'
#define KEY_UP 119 // 'w'
#define KEY_DOWN 115 // 's'
#define KEY_ENTER 10 // ENTER KEY
#define KEY_QUIT 113 // 'q'
#define KEY_ESC 27 // ESC KEY
#define KEY_BACKSPACE 127 //BACKSPACE KEY

enum gameState {STARTMENU, GAME, CREATE_PLAYER, LOAD_PLAYER, ABOUT, MONSTER, MONSTER_FIGHT, MONSTER_STATUS, GAME_OVER, WON, EXIT};
enum gameState stateOfGame;

//Variabler
bool enterPressed;
int menuPos = 3; //standard
static int menuOptions = 4; //Max
static int playerPos = WIDTH/2;
static monster currentMonster = monster();
static std::string tmpName = {};
static std::string windowArr[HEIGHT] = {};
std::default_random_engine generator;
std::vector<character> loadingPlayers {};

keyboard keyboardCTRL;
static bool killKeyboardThread = false;

//SQL
sqlDB DB("saves.db");

//Settings
#define DIVIDER 1000
static int divideCoins = 40;
static int divideMonsters = 10;
static int divideDragons = 1;
static int divideInToCave = 1;


std::string generateLine(character &player) {
    std::string output = {};
    std::uniform_int_distribution<int> distribution(1,1000);
    for(char c = 0; c < WIDTH; ++c) {
        int randomGenerate = distribution(generator);
        if(randomGenerate >= (DIVIDER - divideCoins)) { //Hvis større eller lig med end 950
            output += "C";
        } else if(randomGenerate >= (DIVIDER - divideCoins - divideMonsters) && randomGenerate < (DIVIDER - divideCoins)) { //Hvis større eller lig med 940 og mindre end 950
            output += "M";
        } else if(randomGenerate >= (DIVIDER - divideCoins - divideMonsters - divideDragons) && randomGenerate < (DIVIDER - divideCoins - divideMonsters)) { //Hvis større end eller lig med 939 og mindre end 940
            output += "D";
        } else if(randomGenerate >= (DIVIDER - divideCoins - divideMonsters - divideDragons - divideInToCave) && randomGenerate < (DIVIDER - divideCoins - divideMonsters - divideDragons)) { //Hvis større end eller lig med 938 og mindre end 939
            output += "#";
        } else {
            output += " ";
        }
    }
    return output;
}


void drawGame(character &player, int &pos) {


    std::cout << "[ XP: " << std::setw(6) << std::right << player.getXP() << " Lvl: " << std::setw(2) << player.getLvl() << std::right << " NAME: " << std::setw(13) << std::right  << player.getName() << " ]" << std::endl;
    std::cout << "[ pID: " << std::setw(5) << player.getId() << " pHP: " << std::setw(5) << player.getHp() << " pDMG: " << std::setw(5) << player.getStrength() << std::setw(WIDTH-33) << " ]" /*<< playerPos*/ << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;

    for(int y = 0; y <= HEIGHT; y++) {
        switch(y) {
        case 0:
        {
            char taken {};
            if(windowArr[2].length() >= pos) {
                taken = windowArr[2].at(pos-1); //Det her er den første process der kører. Ergo er den vi skal kigge efter 2 oppe når vi kigger efter den
            }
            if(taken == 'C') {
                int xp = player.getXP();
                player.setXP(xp+(player.getLvl()*100));
                if(player.getHp() < (10+(2*player.getLvl()))) {
                    player.gainHP();
                }
                if(player.getStrength() < (2+player.getLvl())) {
                    player.gainStrength();
                }
                std::cout << '\a';
            }
            if(taken == 'M') {
                std::uniform_int_distribution<int> distribution(1,100);
                int randomLevel = distribution(generator);
                currentMonster = monster(randomLevel);
                std::cout << '\a';
                menuPos = 0;
                stateOfGame = MONSTER;
            }
            if(taken == 'D') {
                currentMonster = monster(100, "DRAGON BOSS");
                std::cout << '\a';
                stateOfGame = MONSTER_FIGHT;
                menuPos = 0;
            }
            break;
        }
        case HEIGHT:
            windowArr[y] = generateLine(player);
            break;
        default:
            windowArr[y] = windowArr[y+1];
        }
    }

    for(int y = HEIGHT; y >= 0; --y) {

        switch(y) {
        case 0:
            std::cout << "[" << std::setw(pos) << "H" << std::setw(WIDTH - pos+1) << "]" << std::endl;
            break;
        default:
            std::cout << "[" << windowArr[y] << "]" << std::endl;
        }


    }

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
        stateOfGame = GAME;
    }

    if(player.getHp() <= 0) {
        if(!DB.killHero(player)) { //Hvis SQL fejler. Så man kan se fejlen
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
        player = character();

        stateOfGame = GAME_OVER;
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
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
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            for(int y = 0; y <= HEIGHT; ++y){ //Start med en tom bane
                windowArr[y].clear();
                for(int x = 0; x < WIDTH; ++x){
                    windowArr[y] += ' ';
                }
            }
            stateOfGame = GAME;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // for errordetection
            stateOfGame = STARTMENU;
        }


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

        //player->setXP(1000);
        for(int y = 0; y <= HEIGHT; ++y){ //Emptying the field
            windowArr[y].clear();
            for(int x = 0; x < WIDTH; ++x){
                windowArr[y] += ' ';
            }
        }

        stateOfGame = GAME;
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
                        bogstav = 0; //DEBUG
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
                    if(playerPos > 1) { --playerPos; }
                    break;
                case KEY_RIGHT:
                    if(playerPos < WIDTH) { ++playerPos; }
                    break;
                case KEY_QUIT:
                case KEY_ESC:
                    menuPos = 3; //Top af menu
                    stateOfGame = STARTMENU;
                    if(DB.saveHero(player)) {
                        std::cout << "GAME SAVED" << std::endl;
                    } else {
                        std::cout << "GAME NOT SAVED - CHECK SQL" << std::endl;
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
                case KEY_LEFT:
                    if(playerPos > 0) { --playerPos; }
                    break;
                case KEY_RIGHT:
                    if(playerPos < WIDTH) { ++playerPos; }
                    break;
                case KEY_UP:
                    if(menuPos < (menuOptions-1)) { ++menuPos; }
                    break;
                case KEY_DOWN:
                    if(menuPos > 0) { --menuPos; }
                    break;
                case KEY_ENTER:
                    enterPressed = true;
                    break;
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
    std::system("clear"); //cls for windows
    std::cout << "KILL THE DRAGON... booting up" << std::endl;
    srand(time(0));
    stateOfGame = STARTMENU;
    for(int y = 0; y <= HEIGHT; ++y){
        for(int x = 0; x < WIDTH; ++x){
            windowArr[y] += ' ';
        }
    }

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

    std::thread keyboardThread(keyboardCTRLFunc, std::ref(player));

    do
    {
        std::system("clear"); //cls for windows
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
            drawGame(player, playerPos);
            break;
        case MONSTER:
            menuOptions = 2;
            drawMonsterConflict(player, currentMonster);
            break;
        case MONSTER_FIGHT:
            menuOptions = 2; //Jeg bruger den selvom det ikke er noget man selv kan styre i spillet
            drawMonsterFight(player, currentMonster);
            break;
        case GAME_OVER:
            drawGameOver();
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
