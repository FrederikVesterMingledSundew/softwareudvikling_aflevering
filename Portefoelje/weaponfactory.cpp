#include "weaponfactory.h"
#include "cavefactory.h"
#include "SQLdatabase.h"
#include <random>
#include <string>

std::default_random_engine weaponGenerator;

WeaponFactory::WeaponFactory() {}

Weapon WeaponFactory::generateWeapon(int caveLvl) {


    //std::uniform_int_distribution<int> distribution(0, caveLvl); //
    std::uniform_int_distribution<int> distribution(0, size(weaponType));

    int weaponLvl = ((double)caveLvl/(double)(MAX_MONSTER_LEVEL*MAX_MONSTERS_IN_CAVE))*size(materialNames);
    int weaponTypeMod = distribution(weaponGenerator); //Talværdi for hvad våben det er

    std::string weaponName = materialNames[weaponLvl]+" "+weaponType[weaponTypeMod];

    Weapon output = Weapon(weaponName, weaponLvl, (weaponLvl*(2/3)), weaponTypeMod*weaponLvl, weaponLvl*100);

    return output;
}

Weapon WeaponFactory::buyWeapon(int weaponID) {
    return Weapon("", 0, 0, 0, 0);
}
