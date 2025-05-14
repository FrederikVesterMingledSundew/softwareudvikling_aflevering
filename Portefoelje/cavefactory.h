#ifndef CAVEFACTORY_H
#define CAVEFACTORY_H

#include "caves.h"
#include "monsterfactory.h"
#include <string>

static std::string caveModifierNames[10] = {

    "Grim",
    "Savage",
    "Abyssal",
    "Cursed",
    "Forlorn",
    "Vile",
    "Shadowed",
    "Wretched",
    "Infernal",
    "Dread"

};

class CaveFactory
{
public:
    CaveFactory();

    cave createCave(int lvl);
};

#endif // CAVEFACTORY_H
