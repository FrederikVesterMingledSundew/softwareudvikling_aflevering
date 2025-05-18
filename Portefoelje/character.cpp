#include "character.h"

character::character() {};

character::character(std::string name, int ID): mName(name), mID(ID) {}

character::character(std::string name, int ID, int xp, int lvl, int coins, int Hp, int Strength):mName(name), mID(ID), mXP(xp), mLvl(lvl), mCoins(coins), mHp(Hp), mStrength(Strength) {}

character::~character() {
    if(mWeapon != nullptr) {
        delete(mWeapon);
    }
};

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

    mXP -= mLvl*1000;
    ++mLvl;

    mHp = 10+(2*mLvl);
    mStrength = 2+mLvl;
}

const int character::getHp() const {
    return mHp;
}

void character::gainHP() {
    ++this->mHp;
}

void character::gainStrength() {
    ++this->mStrength;
}


const int character::getStrength() const {
    return mStrength;
}

void character::hit(const int &damage) {
    this->mHp -= damage;
}

const int character::getId() const {
    return mID;
}

const std::string character::getName() const {
    return mName;
}

const int character::getPlayerPos() const {
    return mPos;
}
void character::movePlayer(int pos) {
    mPos += pos;
}
void character::setPlayerPos(int pos) {
    mPos = pos;
}

const int character::getCoins() const {
    return mCoins;
}

void character::addCoins(int coinsToAdd) {
    mCoins += coinsToAdd;
}

void character::setCoins(int coinsToAdd) {
    mCoins = coinsToAdd;
}

void  character::setWeapon(Weapon input) {
    if(mWeapon != nullptr) {
        delete(mWeapon);
    }

    mWeapon = new Weapon(input);
}

const Weapon*  character::getWeapon() const {
    return mWeapon;
}
