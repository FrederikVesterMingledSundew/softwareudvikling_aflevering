#include "monster.h"

monster::monster() {}

monster::monster(int lvl, std::string name): mLevel(lvl), mName(name) {
    mHp = 10+(2*lvl);
    mDamage = 2+lvl;
    winXP = mHp + mDamage + lvl;
}

const int monster::getLvl() const {
    return mLevel;
}

const int monster::getHp() const {
    return mHp;
}

const int monster::getDamage() const {
    return mDamage;
}

const std::string monster::getName() const {
    return mName;
}

void monster::hit(const int &damage) {
    mHp -= damage;
}

const int monster::getWinXP() const {
    return winXP;
}

const bool monster::isDragon() const {
    return mIsDragon;
}
void monster::setDragon(bool isDragon) {
    mIsDragon = isDragon;
}
