/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include <window.h>
#include <strings.h>
#include <log.h>
#include <object_manager.h>

using namespace std;

void Window::build_scrolling_buttons(){
    if(scrolling_buttons.length()>0){
        //Erase any previously existing scrolling buttons
        while(buttons.size()>last_normal_button+1){
            buttons.pop_back();
        }

        scroll_offset=0;

        if(scrolling_buttons=="configure_commands"){
            Object_Manager::add_game_command_scrolling_button(buttons);
        }
        else if(scrolling_buttons=="server_list"){
            for(int i=0;i<Network_Client::server_list.size();i++){
                buttons.push_back(Button());
                buttons[buttons.size()-1].x=Object_Manager::get_font(font)->spacing_x*2;
                buttons[buttons.size()-1].y=0;
                buttons[buttons.size()-1].start_x=buttons[buttons.size()-1].x;
                buttons[buttons.size()-1].start_y=buttons[buttons.size()-1].y;
                buttons[buttons.size()-1].text=Network_Client::server_list[i].get_button_text();
                buttons[buttons.size()-1].font="small";
                buttons[buttons.size()-1].event_function="server_list_"+Strings::num_to_string(i);
                buttons[buttons.size()-1].set_dimensions();
            }
        }
        else if(scrolling_buttons=="server_list_delete"){
            for(int i=0;i<Network_Client::server_list.size();i++){
                buttons.push_back(Button());
                buttons[buttons.size()-1].x=Object_Manager::get_font(font)->spacing_x*2;
                buttons[buttons.size()-1].y=0;
                buttons[buttons.size()-1].start_x=buttons[buttons.size()-1].x;
                buttons[buttons.size()-1].start_y=buttons[buttons.size()-1].y;
                buttons[buttons.size()-1].text=Network_Client::server_list[i].get_button_text();
                buttons[buttons.size()-1].font="small";
                buttons[buttons.size()-1].event_function="server_list_delete_"+Strings::num_to_string(i);
                buttons[buttons.size()-1].set_dimensions();
            }
        }
        else if(scrolling_buttons=="server_list_edit"){
            for(int i=0;i<Network_Client::server_list.size();i++){
                buttons.push_back(Button());
                buttons[buttons.size()-1].x=Object_Manager::get_font(font)->spacing_x*2;
                buttons[buttons.size()-1].y=0;
                buttons[buttons.size()-1].start_x=buttons[buttons.size()-1].x;
                buttons[buttons.size()-1].start_y=buttons[buttons.size()-1].y;
                buttons[buttons.size()-1].text=Network_Client::server_list[i].get_button_text();
                buttons[buttons.size()-1].font="small";
                buttons[buttons.size()-1].event_function="server_list_edit_"+Strings::num_to_string(i);
                buttons[buttons.size()-1].set_dimensions();
            }
        }
        else if(scrolling_buttons=="lan_server_list"){
            for(int i=0;i<Network_LAN_Browser::lan_server_list.size();i++){
                buttons.push_back(Button());
                buttons[buttons.size()-1].x=Object_Manager::get_font(font)->spacing_x*2;
                buttons[buttons.size()-1].y=0;
                buttons[buttons.size()-1].start_x=buttons[buttons.size()-1].x;
                buttons[buttons.size()-1].start_y=buttons[buttons.size()-1].y;
                buttons[buttons.size()-1].text=Network_LAN_Browser::lan_server_list[i].get_button_text();
                buttons[buttons.size()-1].tooltip_text="hold Control (or the Left Shoulder button on a gamepad) when clicking on a server to add it to the server list";
                buttons[buttons.size()-1].font="small";
                buttons[buttons.size()-1].event_function="lan_server_list_"+Strings::num_to_string(i);
                buttons[buttons.size()-1].alt_function1="lan_server_list_save_"+Strings::num_to_string(i);
                buttons[buttons.size()-1].set_dimensions();
            }
        }
        else{
            Log::add_error("Invalid scrolling buttons list: '"+scrolling_buttons+"'");
        }
    }
}
