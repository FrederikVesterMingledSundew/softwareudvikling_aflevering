#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#define START_HP 5
#define START_STRENGTH 5

class character
{
public:
    character();
    character(std::string name, int ID);
    character(std::string name, int ID, int xp, int lvl, int mHp, int mStrength);
    const int getXP() const;
    void setXP(int xp);
    const int getLvl() const;
    void lvlUp();
    const int getId() const;
    const std::string getName() const;

private:
    int mXP = 0, mLvl = 0, mHp = START_HP, mStrength = START_STRENGTH, mID = -1;
    std::string mName {};
};

#endif // CHARACTER_H
