#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>

class character
{
public:
    character(std::string name);
    character(std::string name, int xp, int lvl);
    const int getXP() const;
    void setXP(int xp);
    const int getLvl() const;
    void lvlUp();
    const std::string getName() const;

private:
    int mXP = 0, mLvl = 0;
    std::string mName {};
};

#endif // CHARACTER_H
