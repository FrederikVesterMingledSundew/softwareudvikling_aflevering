#include "character.h"

character::character(std::string name):mName(name) {}

character::character(std::string name, int xp, int lvl):mName(name), mXP(xp), mLvl(lvl) {}

const int character::getXP() const {
    return mXP;
}

void character::setXP(int xp) {
    mXP = xp;
}

const int character::getLvl() const {
    return mLvl;
}

void character::lvlUp() {
    ++mLvl;
    mXP -= mLvl*1000;
}

const std::string character::getName() const {
    return mName;
}
