/* Copyright (c) Cheese and Bacon Games, LLC. See docs/LICENSE.md for details. */

#include "game_options.h"

using namespace std;

// int Game_Options::example_option=0;

bool Game_Options::get_option (string name, string& value) {
    /**if(name=="cl_example_option"){
        value=Strings::num_to_string(example_option);

        return true;
       }*/

    return false;
}

void Game_Options::set_option (string name, string value) {
    /**if(name=="cl_example_option"){
        example_option=Strings::string_to_long(value);
       }*/
}
