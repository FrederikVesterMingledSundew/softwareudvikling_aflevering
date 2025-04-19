#include "monster.h"
#include <random>

std::default_random_engine monsterGenerator;

monster::monster() {}

monster::monster(int lvl): mLevel(lvl) {
    std::uniform_int_distribution<int> distribution(0,49);
    int choiceOfName = distribution(monsterGenerator);
    switch(lvl) {
    case 0 ... 15:
        mName = monsterStrength[0];
        break;
    case 16 ... 30:
        mName = monsterStrength[1];
        break;
    case 31 ... 45:
        mName = monsterStrength[2];
        break;
    case 46 ... 60:
        mName = monsterStrength[3];
        break;
    case 61 ... 75:
        mName = monsterStrength[4];
        break;
    case 76 ... 100:
        mName = monsterStrength[5];
        break;
    default:
        mName = "Default";
    }

    mHp = 10+(2*lvl);
    mDamage = 2+lvl;
    winXP = mHp + mDamage + lvl;
    mName += " "+monsterNames[choiceOfName];
}

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

/*

public:
    monster();
    monster(int lvl);
    int getLvl();
    int getHp();
    int getDamage();
    std::string getName();

private:
    int mLevel = 1, mHp = 10, mDamage = 2;
    std::string monsterNames[50] = {
        "Abaddon", "Ahuizotl", "Adlet", "Afanc", "Basilisk",
        "Chimera", "Kraken", "Nuckelavee", "Selkie", "Wendigo",
        "Hydra", "Cerberus", "Minotaur", "Gorgon", "Cyclops",
        "Fenrir", "Jormungandr", "Leviathan", "Manticore", "Phoenix",
        "Roc", "Sphinx", "Yeti", "Bigfoot", "Chupacabra",
        "Dullahan", "Banshee", "Kelpie", "Kappa", "Oni",
        "Tengu", "Yurei", "Draugr", "Lich", "Ghoul",
        "Zombie", "Vampire", "Werewolf", "Poltergeist", "Shadowfiend",
        "Gremlin", "Imp", "Goblin", "Troll", "Orc",
        "Balrog", "Nazgul", "Wraith", "Specter", "Djinn"
    };

    std::string monsterStrength[6] = { "Weak", "Light", "Intermediate", "Strong", "Stronger", "Godlike"};


*/
