#ifndef CAVES_H
#define CAVES_H

#include <random>
#include <vector>
#include "monster.h"

#define MAX_MONSTERS_IN_CAVE 5

class cave
{
public:
    cave();
    cave(int lvl);
    ~cave() {  }

    std::vector<monster> &getMonsters();

    const int returnGold() const;

    const int getCaveLevel() const;


private:
    std::vector<monster> mCaveMonsters;
    int mCaveLevel, mMonsterCount, mWinGold;
};

#endif // CAVES_H
