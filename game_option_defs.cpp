/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "world.h"

#include <game_option.h>
#include <strings.h>
#include <log.h>
#include <options.h>
#include <controller_manager.h>
#include <game_window.h>

using namespace std;

string Game_Option::get_value(){
    if(name=="cl_effect_limit"){
        return Strings::num_to_string(Options::effect_limit);
    }
    else if(name=="cl_screen_shake"){
        return Strings::bool_to_string(Options::screen_shake);
    }
    else if(name=="cl_rumble"){
        return Strings::bool_to_string(Options::rumble);
    }
    else if(name=="cl_screensaver"){
        return Strings::bool_to_string(Options::screensaver);
    }
    else if(name=="cl_camera_speed"){
        return Strings::num_to_string(Options::camera_speed);
    }
    else if(name=="cl_camera_zoom"){
        return Strings::num_to_string(Options::camera_zoom);
    }
    else if(name=="cl_name"){
        return Options::name;
    }
    else if(name=="cl_chat_timestamps"){
        return Strings::bool_to_string(Options::chat_timestamps);
    }
    else if(name=="cl_network_rate_bytes"){
        return Strings::num_to_string(network.rate_bytes);
    }
    else if(name=="cl_network_rate_updates"){
        return Strings::num_to_string(network.rate_updates);
    }
    else if(name=="cl_network_rate_commands"){
        return Strings::num_to_string(network.rate_commands);
    }
    else if(name=="sv_max_players"){
        return Strings::num_to_string(Options::max_players);
    }
    else if(name=="sv_network_max_clients"){
        return Strings::num_to_string(network.max_clients);
    }
    else if(name=="sv_network_port"){
        return Strings::num_to_string(network.port);
    }
    else if(name=="sv_network_password"){
        return network.password;
    }
    else if(name=="sv_network_name"){
        return network.name;
    }
    else if(name=="sv_network_frequent_connection_protection"){
        return Strings::bool_to_string(network.frequent_connection_protection);
    }
    else if(name=="sv_network_ignore_checksum"){
        return Strings::bool_to_string(network.ignore_checksum);
    }
    else if(name=="sv_network_rate_bytes_min"){
        return Strings::num_to_string(network.rate_bytes_min);
    }
    else if(name=="sv_network_rate_bytes_max"){
        return Strings::num_to_string(network.rate_bytes_max);
    }
    else if(name=="sv_network_rate_updates_min"){
        return Strings::num_to_string(network.rate_updates_min);
    }
    else if(name=="sv_network_rate_updates_max"){
        return Strings::num_to_string(network.rate_updates_max);
    }
    else{
        Log::add_error("Invalid game option: '"+name+"'");

        return "";
    }
}

void Game_Option::set_value(string new_value){
    if(name=="cl_effect_limit"){
        Options::effect_limit=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="cl_screen_shake"){
        Options::screen_shake=Strings::string_to_bool(new_value);
    }
    else if(name=="cl_rumble"){
        Options::rumble=Strings::string_to_bool(new_value);

        if(!Options::rumble){
            Controller_Manager::stop_rumble(Controller_Manager::CONTROLLER_ID_ALL);
        }
    }
    else if(name=="cl_screensaver"){
        Options::screensaver=Strings::string_to_bool(new_value);

        Game_Window::set_sdl_hints();
    }
    else if(name=="cl_camera_speed"){
        Options::camera_speed=Strings::string_to_double(new_value);
    }
    else if(name=="cl_camera_zoom"){
        Options::camera_zoom=Strings::string_to_double(new_value);
    }
    else if(name=="cl_name"){
        string old_name=Options::name;

        Options::name=new_value;

        network.send_name_change(old_name,Options::name,true);
        network.send_client_data();
    }
    else if(name=="cl_chat_timestamps"){
        Options::chat_timestamps=Strings::string_to_bool(new_value);
    }
    else if(name=="cl_network_rate_bytes"){
        network.rate_bytes=Strings::string_to_unsigned_long(new_value);

        network.send_client_data();
    }
    else if(name=="cl_network_rate_updates"){
        network.rate_updates=Strings::string_to_unsigned_long(new_value);

        network.send_client_data();
    }
    else if(name=="cl_network_rate_commands"){
        network.rate_commands=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="sv_max_players"){
        Options::max_players=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_max_clients"){
        network.max_clients=Strings::string_to_unsigned_long(new_value);

        network.update_server_max_connections(network.max_clients,false);
    }
    else if(name=="sv_network_port"){
        network.port=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_password"){
        if(new_value.length()>32){
            new_value.resize(32);
        }

        network.password=new_value;

        network.update_server_password();
    }
    else if(name=="sv_network_name"){
        if(new_value.length()>48){
            new_value.resize(48);
        }

        network.name=new_value;

        network.update_offline_ping_response();
    }
    else if(name=="sv_network_frequent_connection_protection"){
        network.frequent_connection_protection=Strings::string_to_bool(new_value);
    }
    else if(name=="sv_network_ignore_checksum"){
        network.ignore_checksum=Strings::string_to_bool(new_value);
    }
    else if(name=="sv_network_rate_bytes_min"){
        network.rate_bytes_min=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_rate_bytes_max"){
        network.rate_bytes_max=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_rate_updates_min"){
        network.rate_updates_min=Strings::string_to_unsigned_long(new_value);
    }
    else if(name=="sv_network_rate_updates_max"){
        network.rate_updates_max=Strings::string_to_unsigned_long(new_value);
    }
}
