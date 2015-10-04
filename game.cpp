/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "game.h"

#include <game_world.h>
#include <render.h>
#include <game_window.h>
#include <sound_manager.h>

using namespace std;

///vector<Example_Object> Game::example_objects;

void Game_World::clear_world(){
    ///Game::example_objects.clear();
}

void Game_World::generate_world(){
    clear_world();
}

void Game_World::tick(){
}

void Game_World::ai(){
}

void Game_World::movement(){
}

void Game_World::events(){
    ///Sound_Manager::set_listener(Game::example_player.circle.x,Game::example_player.circle.y,Game_Manager::camera_zoom);
}

void Game_World::animate(){
}

void Game_World::render(){
}

void Game_World::render_to_textures(){
    /**Rtt_Manager::set_render_target("example");
    ///Render something here
    Rtt_Manager::reset_render_target();*/
}

void Game_World::update_background(){
}

void Game_World::render_background(){
    Render::render_rectangle(0,0,Game_Window::width(),Game_Window::height(),1.0,"ui_black");
}
