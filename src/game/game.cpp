/* Copyright (c) Cheese and Bacon Games, LLC. See docs/LICENSE.md for details. */

#include "game.h"

#include <render/render.h>
#include <game_window.h>
#include <sound/sound_manager.h>

using namespace std;

// vector<Example_Object> Game::example_objects;

void Game::clear_world () {
    // example_objects.clear();
}

void Game::generate_world () {
    clear_world();
}

void Game::tick () {
}

void Game::ai () {
}

void Game::movement () {
}

void Game::events () {
    // Sound_Manager::set_listener(example_player.circle.x,example_player.circle.y,Game_Manager::camera_zoom);
}

void Game::animate () {
}

void Game::render () {
}

void Game::render_to_textures () {
    /**Rtt_Manager::set_render_target("example");
       // Render something here Rtt_Manager::reset_render_target();*/
}

void Game::update_background () {
}

void Game::render_background () {
    Render::render_rectangle(0.0, 0.0, Game_Window::width(), Game_Window::height(), 1.0, "ui_black");
}
