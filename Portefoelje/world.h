#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>
#include <random>
#include <stdexcept>

//Settings
#define DIVIDER 1000

//extern std::default_random_engine generatorRand;

class World
{
public:
    World( int length, int height );

    void clearField();

    std::string getLine(int which);

    std::string generateLine();

    void newLine();

    void moveLines(int line);

private:
    int size[2]; //X og Y
    std::vector<std::string> windowArr = {};

};

#endif // WORLD_H
