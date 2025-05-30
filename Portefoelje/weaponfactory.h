#ifndef WEAPONFACTORY_H
#define WEAPONFACTORY_H

#include "cavefactory.h"
#include "SQLdatabase.h"
#include "weapon.h"

#define WEAPON_LVL_TO_PRICE_CONST 100

static std::string materialNames[] = { //12
    "Rubber",
    "Plastic",
    "Wooden",
    "Tin",
    "Aluminium",
    "Bronze",
    "Iron",
    "Steel",
    "Titanium",
    "Tungsten",
    "Tungsten Carbide",
    "Diamond",
    "SuperDiamond"
};

static std::string weaponType[] = { //31
    "Blade",
    "Axe",
    "Spear",
    "Bow",
    "Dagger",
    "Mace",
    "Staff",
    "Hammer",
    "Flail",
    "Lance",
    "Scythe",
    "Katana",
    "Club",
    "Trident",
    "Rapier",
    "Crossbow",
    "Halberd",
    "Whip",
    "Saber",
    "Javelin",
    "Glaive",
    "Kukri",
    "Shuriken",
    "Chakram",
    "Broadsword",
    "Sai",
    "Scimitar",
    "Warhammer",
    "Longsword",
    "Scepter",
    "Gun"
};

class WeaponFactory
{
public:
    WeaponFactory();

    Weapon generateWeapon(int caveLvl, sqlDB &DB);
};

#endif // WEAPONFACTORY_H
