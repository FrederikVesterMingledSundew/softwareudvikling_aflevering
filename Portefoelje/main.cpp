#include <iostream>
#include <iomanip>
#include <cstdlib> // For system()
#include <unistd.h>
#include <chrono>
#include <thread>
#include <string>
#include <random>
#include <vector>

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

enum gameState {STARTMENU, GAME, CREATE_PLAYER, LOAD_PLAYER, MONSTER, MONSTER_STATUS, GAME_OVER, WON, EXIT};
enum gameState stateOfGame;

//Variabler
bool enterPressed;
int menuPos = 3; //standard
static int menuOptions = 4; //Max
static int playerPos = WIDTH/2;
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


std::string generateLine(character &player) {
    std::string output = {};
    std::uniform_int_distribution<int> distribution(1,1000);
    for(char c = 0; c < WIDTH; ++c) {
        int randomGenerate = distribution(generator);;
        if(randomGenerate >= (DIVIDER - divideCoins)) { //Hvis større eller lig med end 950
            output += "C";
        } else if(randomGenerate >= (DIVIDER - divideCoins - divideMonsters) && randomGenerate < (DIVIDER - divideCoins)) { //Hvis større eller lig med 940 og mindre end 950
            output += "M";
        } else if(randomGenerate >= (DIVIDER - divideCoins - divideMonsters - divideDragons) && randomGenerate < (DIVIDER - divideCoins - divideMonsters)) { //Hvis større end eller lig med 939 og mindre end 940
            output += "D";
        } else {
            output += " ";
        }
    }
    return output;
}


void drawGame(character &player, int &pos) {
    std::cout << "[ XP: " << std::setw(6) << std::right << player.getXP() << " Lvl: " << std::setw(2) << player.getLvl() << std::right << " NAME: " << std::setw(13) << std::right  << player.getName() << " ]" << std::endl;
    std::cout << "[ pID: " << std::setw(34) << player.getId() << "]" << std::endl;
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
                player.setXP(xp+1);
                std::cout << '\a';
            }
            if(taken == 'M') {
                std::cout << '\a';
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

void drawMenu() {
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH/2-1) << std::left << "KILL THE DRAGON" << std::setw(WIDTH/2) << "" << "]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    std::cout << "[ " << std::setw(WIDTH-1) << "Controls:" << "]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "LEFT(GAME): A, RIGHT(GAME): D" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "UP(MENU): W, DOWN(MENU): S" << " ]" << std::endl;
    std::cout << "[ " << std::setw(WIDTH-2) << "ENTER(MENU): ENTER" << " ]" << std::endl;
    std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;

    switch(menuPos) {
    case 3:
        std::cout << "[ " << std::setw(WIDTH-15) << std::right << "->LOAD GAME<-" << std::setw(13) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "NEW GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "LEVEL UP" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "EXIT GAME" << std::setw(15) << "" << " ]" << std::endl;
        break;
    case 2:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "LOAD GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-15) << "->NEW GAME<-" << std::setw(13) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "LEVEL UP" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "EXIT GAME" << std::setw(15) << "" << " ]" << std::endl;
        break;
    case 1:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "LOAD GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "NEW GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-15) << "->LEVEL UP<-" << std::setw(13) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "EXIT GAME" << std::setw(15) << "" << " ]" << std::endl;
        break;
    case 0:
        std::cout << "[ " << std::setw(WIDTH-17) << std::right << "LOAD GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "NEW GAME" << std::setw(15) << "" << " ]" << std::endl;
        std::cout << "[ " << std::setw(WIDTH-17) << "LEVEL UP" << std::setw(15) << "" << " ]" << std::endl;
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
            stateOfGame = EXIT;
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

void drawPlayerCreationMenu( character*& player ) {
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

            if (player != nullptr) {
                delete player;
             }

            player = new character(tmpName, sqlResult);
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            stateOfGame = GAME;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            stateOfGame = STARTMENU;
        }


    }
}

void drawLoadPlayerMenu( character*& player ) {



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

        character *lastptr = nullptr;
        if (player != nullptr) {
            lastptr = player;
            delete lastptr;
        }

        player = &loadingPlayers[menuPos];

        //player->setXP(1000);


        stateOfGame = GAME;
    }
}

void keyboardCTRLFunc( character*& player ) {

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
                    if( player != nullptr ) {
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
                    if(playerPos > 0) { --playerPos; }
                    break;
                case KEY_RIGHT:
                    if(playerPos < WIDTH) { ++playerPos; }
                    break;
                case KEY_QUIT:
                case KEY_ESC:
                    menuPos = 0;
                    stateOfGame = STARTMENU;
                    std::cout << "[DEBUG]: KEY_ESC pressed" << std::endl;
                    //goto exitLoop_1;
                    break;

                default:
                    std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;
                    ;
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
            else if(stateOfGame == MONSTER) {
                switch(bogstav) {
                case 'y':
                case 'Y':
                    stateOfGame == MONSTER_STATUS; // FIGHT
                    break;
                case 'n':
                case 'N':
                    // FLEE
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
                    if( player != nullptr ) {
                        stateOfGame = GAME;
                        bogstav = 0; //DEBUG
                    }
                    break;
                case KEY_ENTER:
                    enterPressed = true;
                    break;

                }
            }
            else if(stateOfGame == GAME_OVER) {
                switch(bogstav) {
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
    character* player = nullptr;

    std::thread keyboardThread(keyboardCTRLFunc, std::ref(player));

    do
    {
        std::system("clear"); //cls for windows
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
        case GAME:
            drawGame(*player, playerPos);
            break;
        case EXIT:
            goto leaveGame;
            break;
        /*case GAME_OVER:
            goto leaveGame;
            break;*/
        default:
            std::cout << "[DEBUG]: ERROR !" << std::endl;
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


    if (player != nullptr) {
        delete player;
        player = nullptr;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}
