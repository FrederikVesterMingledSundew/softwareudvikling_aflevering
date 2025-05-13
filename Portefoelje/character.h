#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#define START_HP 10
#define START_STRENGTH 2
#define START_LEVEL 1

class character
{
public:
    character();
    character(std::string name, int ID);
    character(std::string name, int ID, int xp, int lvl, int coins, int mHp, int mStrength);
    const int getXP() const;
    void setXP(int xp);
    const int getLvl() const;
    void lvlUp();
    const int getHp() const;
    void gainHP();
    const int getStrength() const;
    void gainStrength();
    void hit(const int &damage);
    const int getId() const;
    const std::string getName() const;
    const int getPlayerPos() const;
    void movePlayer(int pos);
    void setPlayerPos(int pos);
    const int getCoins() const;
    void addCoins(int coinsToAdd);
    void setCoins(int coinsToAdd);


private:
    int mXP = 0, mLvl = START_LEVEL, mHp = START_HP, mStrength = START_STRENGTH, mID = -1, mPos = 0, mCoins = 0;
    std::string mName {};
};

#endif // CHARACTER_H
