#ifndef WEAPON_H
#define WEAPON_H

#include <string>

/*
 * Dette er våbenklassen.
 * Den hænger sammen med heroklassen
 *
 */

class Weapon
{
public:
    Weapon(const Weapon& copy) {
        this->mName = copy.mName;
        this->mDurability = copy.mDurability;
        this->mHits = copy.mHits;
        this->mModifier = copy.mModifier;
        this->mPrice = copy.mPrice;
    }

    Weapon(std::string name, int durability, int hits, int modifier, int price) : mName(name), mDurability(durability), mHits(hits), mModifier(modifier), mPrice(price) {}; //Så lille en class. Laver ikke en cpp til


    void takeHit() {
        if(mHits < mDurability) {
            ++mHits;
        }
    }

    const int getRemainingHits() const {
        return (mDurability - mHits);
    }

    const int getModifier() const {
        return mModifier;
    }

    const int getPrice() const {
        return mPrice;
    }

    const int getDurability() const {
        return mDurability;
    }

    const std::string getName() const {
        return mName;
    }

    friend class WeaponFactory;

private:
    std::string mName;
    int mDurability, mHits, mModifier, mPrice;
};

#endif // WEAPON_H
