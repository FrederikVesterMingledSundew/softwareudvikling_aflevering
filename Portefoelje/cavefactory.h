#ifndef CAVEFACTORY_H
#define CAVEFACTORY_H

#include "caves.h"
#include "monsterfactory.h"
#include <string>

#define MAX_MONSTERS_IN_CAVE 5

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


/*
 * Det her cave factory. Det er her alle caves bliver produceret.
 */

class CaveFactory
{
public:
    CaveFactory();

    cave createCave(int lvl);
};

#endif // CAVEFACTORY_H
