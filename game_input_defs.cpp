/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "world.h"

#include <game_manager.h>
#include <options.h>
#include <engine.h>

using namespace std;

void Game_Manager::handle_command_states_multiplayer(){
    if(in_progress){
        if(Network_Engine::status=="server"){
            network.prepare_server_input_states();

            for(int i=0;i<network.clients.size();i++){
                if(!paused){
                    //Example multiplayer command state
                    /**if(network.clients[i].game_command_state("some_command")){
                        ///Deal with command state here
                    }*/
                }
            }
        }
    }
}

void Game_Manager::handle_game_commands_multiplayer(){
    if(in_progress){
        if(Network_Engine::status=="server"){
            for(int i=0;i<network.clients.size();i++){
                for(int j=0;j<network.clients[i].command_buffer.size();j++){
                    string command_name=network.clients[i].command_buffer[j];

                    if(!paused){
                        //Example multiplayer command
                        /**if(command_name=="some_command"){
                            ///Deal with command here
                        }*/
                    }
                }

                network.clients[i].command_buffer.clear();
            }
        }
    }
}

void Game_Manager::handle_input_states_gui(){
    int mouse_x=0;
    int mouse_y=0;
    Engine::get_mouse_state(&mouse_x,&mouse_y);

    if(in_progress){
        if(Object_Manager::game_command_state("scoreboard")){
            display_scoreboard=true;
        }
    }
}

void Game_Manager::handle_input_states(){
    int mouse_x=0;
    int mouse_y=0;
    Engine::get_mouse_state(&mouse_x,&mouse_y);

    if(in_progress){
        if(!paused){
            //Example multiplayer command state
            /**if(Object_Manager::game_command_state("some_command")){
                command_states.push_back("some_command");
            }*/
        }
    }
}

bool Game_Manager::handle_game_command_gui(string command_name){
    //Pause the game
    if(command_name=="pause"){
        toggle_pause();

        return true;
    }

    //Example multiplayer pause
    /**if(command_name=="pause"){
        if(Network_Engine::status=="server"){
            toggle_pause();

            network.send_paused();
        }

        return true;
    }*/

    //Toggle chat box
    else if(command_name=="chat"){
        engine_interface.chat.toggle_on();

        return true;
    }

    return false;
}

bool Game_Manager::handle_game_command(string command_name){
    const uint8_t* keystates=SDL_GetKeyboardState(NULL);

    ///DEV COMMANDS
    if(Options::dev && keystates[SDL_SCANCODE_F1]){
        //Example dev command
        /**if(command_name=="some_dev_command"){
            ///Dev command here.

            return true;
        }*/
    }
    ///END OF DEV COMMANDS

    if(!paused){
        //Example command
        /**if(command_name=="some_command"){
            ///Command here

            return true;
        }*/

        //Example multiplayer command input
        /**if(command_name=="some_command"){
            network.add_command(command_name);

            return true;
        }*/
    }

    return false;
}

bool Game_Manager::handle_input_events_gui(){
    bool event_consumed=false;

    if(in_progress){
        const vector<Game_Command>& game_commands=Object_Manager::get_game_commands();

        for(int i=0;i<game_commands.size() && !event_consumed;i++){
            if((Engine::event.type==SDL_CONTROLLERBUTTONDOWN && game_commands[i].button==Engine::event.cbutton.button) ||
               (Engine::event.type==SDL_KEYDOWN && Engine::event.key.repeat==0 && game_commands[i].key==Engine::event.key.keysym.scancode)){
                event_consumed=handle_game_command_gui(game_commands[i].name);
            }
        }
    }

    return event_consumed;
}

bool Game_Manager::handle_input_events(){
    bool event_consumed=false;

    if(in_progress){
        const vector<Game_Command>& game_commands=Object_Manager::get_game_commands();

        for(int i=0;i<game_commands.size() && !event_consumed;i++){
            if((Engine::event.type==SDL_CONTROLLERBUTTONDOWN && game_commands[i].button==Engine::event.cbutton.button) ||
               (Engine::event.type==SDL_KEYDOWN && Engine::event.key.repeat==0 && game_commands[i].key==Engine::event.key.keysym.scancode)){
                event_consumed=handle_game_command(game_commands[i].name);
            }
        }
    }

    return event_consumed;
}
