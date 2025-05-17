#ifndef CAVES_H
#define CAVES_H

#include <vector>
#include <string>
#include "monsterfactory.h"

extern MonsterFactory MFactory;

class cave
{
public:
    cave();
    cave(std::vector<monster> caveMonsters);
    ~cave() {  }

    std::vector<monster> &getMonsters();

    const int returnGold() const;

    const int getCaveLevel() const;

    const std::string getCaveModifierName() const;

    friend class CaveFactory;

private:
    std::vector<monster> mCaveMonsters;
    int mCaveLevel, mMonsterCount, mWinGold;
    std::string mCaveModifierName;
};

#endif // CAVES_H
