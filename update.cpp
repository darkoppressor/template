/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "update.h"
#include "world.h"

#include <music_manager.h>
#include <rtt_manager.h>
#include <controller_manager.h>

using namespace std;

void Update::check_mail(){
    while(Game_Mailman::has_mail()){
        string letter=Game_Mailman::get_letter();

        if(letter=="save_options"){
            engine_interface.save_options();
        }
        else{
            Log::add_error("Game_Mailman received unknown letter: '"+letter+"'");
        }
    }
}

void Update::ai(){
    if(game.in_progress && !game.paused){
        game.world.ai();
    }
}

void Update::input(){
    engine_interface.prepare_for_input();

    game.prepare_for_input();

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
                event_consumed=game.handle_input_events_gui();
            }

            if(!event_consumed && !engine_interface.is_any_window_open()){
                event_consumed=game.handle_input_events();
            }
        }
    }

    engine_interface.handle_input_states();

    if(!engine_interface.console.on && !engine_interface.chat.on && !event_ignore_command_set){
        if(!engine_interface.mutable_info_selected()){
            game.handle_input_states_gui();
        }

        if(!engine_interface.is_any_window_open()){
            game.handle_input_states();
        }
    }

    game.handle_game_commands_multiplayer();
    game.handle_command_states_multiplayer();
}

void Update::tick(){
    if(game.in_progress && !game.paused){
        game.world.tick();
    }
}

void Update::movement(){
    if(game.in_progress && !game.paused){
        game.world.movement();
    }
}

void Update::events(){
    game.manage_music();

    if(game.in_progress && !game.paused){
        game.world.events();
    }
    else{
        Sound_Manager::set_listener(game.camera.center_x(),game.camera.center_y(),game.camera_zoom);
    }
}

void Update::animate(){
    Music_Manager::fadein_tracks();
    Music_Manager::fadeout_tracks();

    engine_interface.animate();

    if(game.in_progress && !game.paused){
        game.world.animate();
    }
}

void Update::camera(int frame_rate,double ms_per_frame,int logic_frame_rate){
    engine_interface.update_window_caption(frame_rate,ms_per_frame,logic_frame_rate);

    if(game.in_progress){
        game.set_camera();

        game.world.update_background();
    }
}

void Update::render(int frame_rate,double ms_per_frame,int logic_frame_rate){
    SDL_SetRenderDrawColor(main_window.renderer,0,0,0,255);
    SDL_RenderClear(main_window.renderer);

    if(game.in_progress){
        /**Rtt_Manager::set_render_target(main_window.renderer,"example");
        ///Render something here
        Rtt_Manager::reset_render_target(main_window.renderer);*/

        game.world.render_background();

        game.world.render();

        game.render_scoreboard();
    }
    else{
        engine_interface.render_title_background();
    }

    engine_interface.render(frame_rate,ms_per_frame,logic_frame_rate);

    SDL_RenderPresent(main_window.renderer);
}
