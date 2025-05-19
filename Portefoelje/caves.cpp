#include "caves.h"


MonsterFactory MFactory;

cave::cave() {}

cave::cave(std::vector<monster> caveMonsters):mCaveMonsters(caveMonsters) {}

std::vector<monster> &cave::getMonsters() {
    return mCaveMonsters;
}

const int cave::returnGold() const {
    return mWinGold;
}

const int cave::getCaveLevel() const {
    return mCaveLevel;
}

const std::string cave::getCaveModifierName() const {
    return mCaveModifierName;
}
