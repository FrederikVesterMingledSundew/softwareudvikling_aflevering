#include "character.h"

character::character() {};

character::character(std::string name, int ID): mName(name), mID(ID) {}

character::character(std::string name, int ID, int xp, int lvl, int Hp, int Strength):mName(name), mID(ID), mXP(xp), mLvl(lvl), mHp(Hp), mStrength(Strength) {}

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

const int character::getHp() const {
    return mHp;
}

const int character::getStrength() const {
    return mStrength;
}

const int character::getId() const {
    return mID;
}

const std::string character::getName() const {
    return mName;
}
