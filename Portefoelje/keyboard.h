#ifndef KEYBOARD_H
#define KEYBOARD_H

/*
 * This was created to replace kbhit and getch on windows, because it has to run on linux.
 * This code is primarily taken from the internet and i claim no ownership of it.
 *
 * https://stackoverflow.com/questions/29335758/using-kbhit-and-getch-on-linux
 */

#include <termios.h>

class keyboard
{
public:
    keyboard();
    ~keyboard();
    int kbhit();
    int getch();

private:
    struct termios initial_settings, new_settings;
    int peek_character;
};

#endif // KEYBOARD_H
