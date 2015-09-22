/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef game_mailman_h
#define game_mailman_h

#include <string>

class Game_Mailman{
public:

    static bool has_mail();
    static std::string get_letter();
};

#endif
