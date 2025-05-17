#ifndef WEAPON_H
#define WEAPON_H

#include <string>

class Weapon
{
public:
    Weapon(std::string name, int durability, int hits, int modifier, int price) : mName(name), mDurability(durability), mHits(hits), mModifier(modifier), mPrice(price) {}; //Så lille en class. Laver ikke en cpp til

    void takeHit() {
        if(mHits < mDurability) {
            ++mHits;
        }
    }

    int getRemainingHits() {
        return (mDurability - mHits);
    }

    int getModifier() {
        return mModifier;
    }

    int getPrice() {
        return mPrice;
    }

    int getDurability() {
        return mDurability;
    }

    friend class WeaponFactory;

private:
    std::string mName;
    int mDurability, mHits, mModifier, mPrice;
};

#endif // WEAPON_H
