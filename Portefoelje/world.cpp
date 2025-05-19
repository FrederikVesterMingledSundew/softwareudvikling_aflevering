#include "world.h"

#include <iostream>

std::default_random_engine generatorRand;

static int divideCoins = 40;
static int divideMonsters = 10;
static int divideDragons = 1;
static int divideInToCave = 1;

World::World(int length, int height) {
    if (length <= 0 || height <= 0) {
        throw std::invalid_argument("Length and height must be positive");
    }
    size[0] = length;
    size[1] = height;
    windowArr.resize(height+1, std::string(length, ' ')); //+1 Fordi hvis du reservere til 20, så er det 0 til 19
};

void World::clearField() {

    for(int y = 0; y < size[1]; ++y){
        //windowArr[y].clear();
        windowArr[y] = std::string(size[0], ' ');

    }

}

std::string World::getLine(int which) {
    if(which > size[1] || which < 0) {
        throw std::invalid_argument("[ERROR]: In World::getLine, \"which\" is out bounds");
        /*std::string output {};
        for(int x = 0; x < size[0]; ++x){
            output += ' ';
        }

        return output;*/
    }
    return windowArr.at(which);
}

std::string World::generateLine() {
    std::string output = {};
    std::uniform_int_distribution<int> distribution(1,DIVIDER);
    for(char c = 0; c < size[0]; ++c) {
        int randomGenerate = distribution(generatorRand);
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

void World::newLine() {
    windowArr[size[1]] = generateLine();
}

void World::moveLines(int line) {
    windowArr[line] = windowArr[line+1];
}
