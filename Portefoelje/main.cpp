#include <iostream>
#include <iomanip>
#include <cstdlib> // For system()
#include <unistd.h>
#include <chrono>
#include <thread>

#include "character.h"
#include "keyboard.h"

// Game settings
#define WIDTH 40
#define HEIGHT 20
#define PADDLE_SIZE 3
#define FRAME_RATE 2

//Keys
#define KEY_LEFT 97 // 'a'
#define KEY_RIGHT 100 // 'd'
#define KEY_ESC 27

enum gameState {STARTMENU, GAME, GAME_OVER};
enum gameState stateOfGame;

static int playerPos = WIDTH/2;
static std::string windowArr[HEIGHT] = {};

keyboard keyboardCTRL;

//Settings
#define pct 100
static int pctMonsters = 2;
static int pctCoins = 10;


std::string generateLine(character &player) {
    std::string output = {};
    for(char c = 0; c < WIDTH; ++c) {
        char randomGenerate = rand() % 100;
        if(randomGenerate >= (pct - pctMonsters)) { //Hvis større end 98
            output += "M";
        } else if(randomGenerate > (pct - pctMonsters - pctCoins) && randomGenerate < (pct - pctMonsters)) {
            output += "C";
        } else {
            output += " ";
        }
    }
    return output;
}


void draw(character &player, int &pos) {

    std::system("clear"); //cls for windows
    std::cout << "[ XP: " << std::setw(6) << std::right << player.getXP() << " Lvl: " << std::setw(2) << player.getLvl() << std::right << " NAME: " << std::setw(13) << std::right  << player.getName() << " ]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;

    for(int y = 0; y <= HEIGHT; y++) {
        switch(y) {
        case 0:
        {
            char taken {};
            if(windowArr[2].length() >= pos) {
                taken = windowArr[2].at(pos-1); //Det her er den første der kører. Ergo er den vi skal kigge efter 2 oppe når vi kigger efter den
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

void keyboardCTRLFunc(character &player) {

    int bogstav;
    do
    {
        bogstav = 0;
        if(keyboardCTRL.kbhit()) {
            bogstav=keyboardCTRL.getch();
            switch(bogstav) {
            case KEY_LEFT:
                if(playerPos > 0) { --playerPos; }
                break;
            case KEY_RIGHT:
                if(playerPos < WIDTH) { ++playerPos; }
                break;
            case KEY_ESC:
                stateOfGame = GAME_OVER;
                //std::cout << "[DEBUG]: KEY_ESC pressed" << std::endl;
                goto exitLoop_1;
                break;

            default:
                //std::cout << "[DEBUG]: KEY pressed: " << bogstav << std::endl;

            }
            fflush(stdin);
            tcflush(0, TCIFLUSH); // Clear stdin to prevent characters appearing on prompt
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while(1);
    exitLoop_1:
    std::cout << "Exit of keyboardCTRLFunc thread" << std::endl;

}

int main()
{
    std::system("clear"); //cls for windows
    srand(time(0));
    stateOfGame = GAME;
    for(int y = 0; y <= HEIGHT; ++y){
        for(int x = 0; x < WIDTH; ++x){
            windowArr[y] += ' ';
        }
    }


    character player = character("Holger");
    player.setXP(1000);

    std::thread keyboardThread(keyboardCTRLFunc, std::ref(player));

    do
    {
        draw(player, playerPos);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FRAME_RATE));
    }
    while(stateOfGame == GAME);

    keyboardThread.join();

    std::cout << "Thank you for playing!" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}
