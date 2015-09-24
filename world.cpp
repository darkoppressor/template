/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "world.h"

#include <engine.h>
#include <image_manager.h>
#include <sound_manager.h>
#include <music_manager.h>
#include <rtt_manager.h>
#include <game_window.h>

using namespace std;

Engine_Interface engine_interface;

bool load_world(){
    GUI_Manager::initialize();

    Data_Manager::load_data_game_options();

    if(!Options::load_options()){
        return false;
    }

    if(!Game_Window::initialize()){
        return false;
    }

    Image_Manager::load_images();

    Sound_Manager::load_sounds();

    Music_Manager::prepare_tracks();

    Data_Manager::load_data_main();

    ///Rtt_Manager::add_texture("example",1024.0,1024.0);

    engine_interface.console.setup(false);
    engine_interface.chat.setup(true);

    if(!Options::load_game_commands()){
        return false;
    }

    Options::load_servers();

    //To be safe, this should be at the very bottom of load_world().
    Image_Manager::set_error_image();

    Engine::world_loaded=true;

    return true;
}
