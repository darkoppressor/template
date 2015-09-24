/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "game_data.h"
#include "game_options.h"

#include <game_manager.h>
#include <game_option.h>

using namespace std;

void Game_Manager::load_data_game(Progress_Bar& bar){
    Game_Data::load_data_game(bar);
}

void Game_Manager::load_data_tag_game(string tag,File_IO_Load* load){
    Game_Data::load_data_tag_game(tag,load);
}

void Game_Manager::unload_data_game(){
    Game_Data::unload_data_game();
}

bool Game_Option::game_get_value(string& value){
    return Game_Options::get_option(name,value);
}

void Game_Option::game_set_value(string value){
    Game_Options::set_option(name,value);
}
