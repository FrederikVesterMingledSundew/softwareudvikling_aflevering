#include "caves.h"

std::default_random_engine randGenerator;

cave::cave() {}

cave::cave(int lvl) {

    std::uniform_int_distribution<int> distribution(1,MAX_MONSTERS_IN_CAVE);
    mMonsterCount = distribution(randGenerator);

    if(lvl > MAX_MONSTER_LEVEL) {
        lvl = MAX_MONSTER_LEVEL;
    }

    std::uniform_int_distribution<int> distribution_2(1,lvl);
    int topLevel = distribution_2(randGenerator), LevelCounter = 0;

    std::uniform_int_distribution<int> distribution_3(1,topLevel);
    for(int i = 0; i < mMonsterCount; ++i) {

        int monsterLevel = distribution_3(randGenerator);
        LevelCounter += monsterLevel;

        mCaveMonsters.push_back(monster(monsterLevel));
        mWinGold += mCaveMonsters[i].getWinXP();

    }
    mCaveLevel = (LevelCounter/mMonsterCount) +1;
}

std::vector<monster> &cave::getMonsters() {
    return mCaveMonsters;
}

const int cave::returnGold() const {
    return mWinGold;
}

const int cave::getCaveLevel() const {
    return mCaveLevel;
}
