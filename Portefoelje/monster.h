#ifndef MONSTER_H
#define MONSTER_H

#include <string>

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

class monster
{
public:
    monster();
    monster(int lvl);
    monster(int lvl, std::string name);
    const int getLvl() const;
    const int getHp() const;
    const int getDamage() const;
    void hit(const int &damage);
    const std::string getName() const;
    const int getWinXP() const;

private:
    int mLevel = 1, mHp = 10, mDamage = 2, winXP = 1;
    std::string mName = "";
};

#endif // MONSTER_H
