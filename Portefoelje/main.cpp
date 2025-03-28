#include <iostream>
#include <iomanip>
#include <cstdlib> // For system()
#include <unistd.h>

#include "character.h"

// Game settings
const int WIDTH = 40;
const int HEIGHT = 20;
const int PADDLE_SIZE = 3;

void draw(character player) {
    std::system("clear");
    std::cout << "[ XP: " << std::setw(6) << std::right << player.getXP() << " Lvl: " << std::setw(2) << player.getLvl() << std::right << " NAME: " << std::setw(13) << std::right  << player.getName() << " ]" << std::endl;
    std::cout << "[----------------------------------------]" << std::endl;
    for(int y = HEIGHT; y > 0; --y) {
        std::cout << "[" << std::setw(WIDTH) << "" << "]" << std::endl;
    }




}

int main()
{
    std::system("clear");
    character player = character("Holger");
    draw(player);

    std::cout << "Hello World!" << std::endl;

    return 0;
}
