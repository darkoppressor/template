/* Copyright (c) Cheese and Bacon Games, LLC. See docs/LICENSE.md for details. */

#ifndef game_options_h
#define game_options_h

#include <string>

class Game_Options {
    public:
        // static int example_option;

        static bool get_option(std::string name, std::string& value);
        static void set_option(std::string name, std::string value);
};

#endif
