#ifndef MONSTERFACTORY_H
#define MONSTERFACTORY_H

#include "character.h"
#include "monster.h"

#define MIN_MONSTER_LEVEL 1
#define MAX_MONSTER_LEVEL 100

#define DRAGON_NAME "DRAGON BOSS"

static std::string monsterNames[50] = {
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

static std::string monsterStrength[6] = { "Weak", "Light", "Intermediate", "Strong", "Stronger", "Godlike"};

class MonsterFactory
{
public:
    MonsterFactory();

    monster createRndMonster();

    monster createDragon();

    monster createMonsterWithLvl(int lvl);

    monster createSpecificMonster(int lvl, std::string name);
};

#endif // MONSTERFACTORY_H
