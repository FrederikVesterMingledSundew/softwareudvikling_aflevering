#include "cavefactory.h"
#include <random>

std::default_random_engine randGenerator;

CaveFactory::CaveFactory() {}

cave CaveFactory::createCave(int lvl) {

    std::vector<monster> caveMonsters;
    int winGold = 0;

    std::uniform_int_distribution<int> distribution(1,MAX_MONSTERS_IN_CAVE);
    int mMonsterCount = distribution(randGenerator);

    if(lvl > MAX_MONSTER_LEVEL) { //Hvis du er for stærk, så bliver monstrene ikke stærkere. Efter lvl 100 kan du nok klare de fleste monstre, men du kan nok ikke klare de fleste caves.
        lvl = MAX_MONSTER_LEVEL;
    }

    std::uniform_int_distribution<int> distribution_2(1,lvl);
    int topLevel = distribution_2(randGenerator), LevelCounter = 0;

    std::uniform_int_distribution<int> distribution_3(1,topLevel);
    for(int i = 0; i < mMonsterCount; ++i) {

        int monsterLevel = distribution_3(randGenerator);
        LevelCounter += monsterLevel;

        caveMonsters.push_back(MFactory.createMonsterWithLvl(monsterLevel));
        winGold += caveMonsters[i].getWinXP();

    }

    cave output = cave(caveMonsters);
    output.mWinGold = winGold; //Friend
    output.mCaveLevel = LevelCounter; //Friend

    if(LevelCounter > MAX_MONSTER_LEVEL) { //Det her er level på caven. Hvis caven er over MAX_LEVEL, så kommer caven til at hedde det maximale
        LevelCounter = MAX_MONSTER_LEVEL;
    }
    switch(LevelCounter) {
    case 1 ... 10:
        output.mCaveModifierName = caveModifierNames[0];
        break;
    case 11 ... 20:
        output.mCaveModifierName = caveModifierNames[1];
        break;
    case 21 ... 30:
        output.mCaveModifierName = caveModifierNames[2];
        break;
    case 31 ... 40:
        output.mCaveModifierName = caveModifierNames[3];
        break;
    case 41 ... 50:
        output.mCaveModifierName = caveModifierNames[4];
        break;
    case 51 ... 60:
        output.mCaveModifierName = caveModifierNames[5];
        break;
    case 61 ... 70:
        output.mCaveModifierName = caveModifierNames[6];
        break;
    case 71 ... 80:
        output.mCaveModifierName = caveModifierNames[7];
        break;
    case 81 ... 90:
        output.mCaveModifierName = caveModifierNames[8];
        break;
    case 91 ... 100:
        output.mCaveModifierName = caveModifierNames[9];
        break;
    default:
        output.mCaveModifierName = "Default";
    }


    return output;
}
