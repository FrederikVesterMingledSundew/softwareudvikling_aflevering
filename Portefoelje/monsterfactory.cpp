#include "monsterfactory.h"
#include <random>
#include <string>

std::default_random_engine monsterGenerator;

MonsterFactory::MonsterFactory() {}

monster MonsterFactory::createRndMonster() {
    std::uniform_int_distribution<int> distribution(MIN_MONSTER_LEVEL, MAX_MONSTER_LEVEL);
    int randomLevel = distribution(monsterGenerator);
    return createMonsterWithLvl(randomLevel);
}

monster MonsterFactory::createMonsterWithLvl(int lvl) { //Random monster

    if(lvl > MAX_MONSTER_LEVEL) {
        lvl = MAX_MONSTER_LEVEL;
    }

    std::uniform_int_distribution<int> distribution(0,49); //Names structure
    int choiceOfName = distribution(monsterGenerator);
    std::string name;
    switch(lvl) {
    case 0 ... 15:
        name = monsterStrength[0];
        break;
    case 16 ... 30:
        name = monsterStrength[1];
        break;
    case 31 ... 45:
        name = monsterStrength[2];
        break;
    case 46 ... 60:
        name = monsterStrength[3];
        break;
    case 61 ... 75:
        name = monsterStrength[4];
        break;
    case 76 ... 100:
        name = monsterStrength[5];
        break;
    default:
        name = "Default";
    }
    name += " "+monsterNames[choiceOfName];

    monster output(lvl, name);

    output.mHp = 10+(2*lvl); //Friend
    output.mDamage = 2+lvl; //Friend
    output.winXP = output.mHp + output.mDamage + lvl; //Friend

    return output;
}

monster MonsterFactory::createDragon() {
    monster output(MAX_MONSTER_LEVEL, DRAGON_NAME);
    output.setDragon(true);
    return output;
}

monster MonsterFactory::createSpecificMonster(int lvl, std::string name) {
    return monster(lvl, name);
}
