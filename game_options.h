/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef game_options_h
#define game_options_h

#include <string>

class Game_Options{
public:

    static std::string get_value(std::string name);

    static void set_value(std::string name,std::string new_value);
};

#endif
