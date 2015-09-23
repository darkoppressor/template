/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "update.h"
#include "world.h"
#include "game_mailman.h"

#include <music_manager.h>
#include <rtt_manager.h>
#include <controller_manager.h>
#include <data_reader.h>
#include <game_window.h>
#include <log.h>
#include <engine.h>
#include <sound_manager.h>

using namespace std;

void Update::check_mail(){
    while(Game_Mailman::has_mail()){
        string letter=Game_Mailman::get_letter();

        if(letter=="save_options"){
            engine_interface.save_options();
        }
        else if(letter=="reload"){
            Game_Window::reload();
        }
        else if(Data_Reader::check_prefix(letter,"console:")){
            engine_interface.console.add_text(letter);
        }
        else{
            Log::add_error("Game_Mailman received unknown letter: '"+letter+"'");
        }
    }
}

void Update::ai(){
    if(Game_Manager::in_progress && !Game_Manager::paused){
        Game_World::ai();
    }
}

void Update::input(){
    engine_interface.prepare_for_input();

    Game_Manager::prepare_for_input();

    bool event_ignore_command_set=false;

    while(engine_interface.poll_event(&Engine::event)){
        bool event_consumed=false;

        if(Engine::event.type==SDL_QUIT){
            engine_interface.quit();

            event_consumed=true;
        }

        if(!event_consumed){
            event_consumed=engine_interface.handle_input_events_drag_and_drop();
        }

        if(!event_consumed && Controller_Manager::touch_controls){
            event_consumed=engine_interface.handle_input_events_touch();
        }

        if(!event_consumed){
            event_consumed=engine_interface.handle_input_events_command_set();
        }

        //If we are still binding a command input.
        if(engine_interface.configure_command!=-1){
            event_ignore_command_set=true;
        }
        else{
            event_ignore_command_set=false;
        }

        if(!event_consumed){
            event_consumed=engine_interface.handle_input_events(event_ignore_command_set);
        }

        if(!engine_interface.console.on && !engine_interface.chat.on && !event_ignore_command_set){
            if(!event_consumed && !engine_interface.mutable_info_selected()){
                event_consumed=Game_Manager::handle_input_events_gui();
            }

            if(!event_consumed && !engine_interface.is_any_window_open()){
                event_consumed=Game_Manager::handle_input_events();
            }
        }
    }

    engine_interface.handle_input_states();

    if(!engine_interface.console.on && !engine_interface.chat.on && !event_ignore_command_set){
        if(!engine_interface.mutable_info_selected()){
            Game_Manager::handle_input_states_gui();
        }

        if(!engine_interface.is_any_window_open()){
            Game_Manager::handle_input_states();
        }
    }

    Game_Manager::handle_game_commands_multiplayer();
    Game_Manager::handle_command_states_multiplayer();
}

void Update::tick(){
    if(Game_Manager::in_progress && !Game_Manager::paused){
        Game_World::tick();
    }
}

void Update::movement(){
    if(Game_Manager::in_progress && !Game_Manager::paused){
        Game_World::movement();
    }
}

void Update::events(){
    Game_Manager::manage_music();

    if(Game_Manager::in_progress && !Game_Manager::paused){
        Game_World::events();
    }
    else{
        Sound_Manager::set_listener(Game_Manager::camera.center_x(),Game_Manager::camera.center_y(),Game_Manager::camera_zoom);
    }
}

void Update::animate(){
    Music_Manager::fadein_tracks();
    Music_Manager::fadeout_tracks();

    engine_interface.animate();

    if(Game_Manager::in_progress && !Game_Manager::paused){
        Game_World::animate();
    }
}

void Update::camera(int frame_rate,double ms_per_frame,int logic_frame_rate){
    engine_interface.update_window_caption(frame_rate,ms_per_frame,logic_frame_rate);

    if(Game_Manager::in_progress){
        Game_Manager::set_camera();

        Game_World::update_background();
    }
}

void Update::render(int frame_rate,double ms_per_frame,int logic_frame_rate){
    Game_Window::clear_renderer(Color(0,0,0,255));

    if(Game_Manager::in_progress){
        /**Rtt_Manager::set_render_target("example");
        ///Render something here
        Rtt_Manager::reset_render_target();*/

        Game_World::render_background();

        Game_World::render();

        Game_Manager::render_scoreboard();
    }
    else{
        engine_interface.render_title_background();
    }

    engine_interface.render(frame_rate,ms_per_frame,logic_frame_rate);

    Game_Window::render_present();
}
