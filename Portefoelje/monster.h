#ifndef MONSTER_H
#define MONSTER_H

#include <string>

/*
 * Det her er monster klassen
 * Det er alle de fjender du møder. Også dragen
 *
 */

class monster
{
public:
    monster();
    //monster(int lvl);
    monster(int lvl, std::string name);
    const int getLvl() const;
    const int getHp() const;
    const int getDamage() const;
    void hit(const int &damage);
    const std::string getName() const;
    const int getWinXP() const;
    const bool isDragon() const;
    void setDragon(bool isDragon);

    friend class MonsterFactory;

private:
    int mLevel = 1, mHp = 10, mDamage = 2, winXP = 1;
    std::string mName = "";
    bool mIsDragon = false;
};

#endif // MONSTER_H
