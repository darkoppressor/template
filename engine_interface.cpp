/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "engine_interface.h"
#include "world.h"

#include <engine_math.h>
#include <collision.h>
#include <strings.h>
#include <log.h>
#include <directories.h>
#include <options.h>
#include <data_reader.h>
#include <sorting.h>
#include <object_manager.h>
#include <render.h>
#include <engine_data.h>
#include <sound_manager.h>
#include <controller_manager.h>
#include <engine.h>
#include <game_window.h>
#include <engine_mailman.h>

#include <cmath>

#include <SDL_image.h>

#include <boost/algorithm/string.hpp>

using namespace std;

bool Engine_Interface::poll_event(SDL_Event* event_storage){
    if(SDL_PollEvent(event_storage)){
        return true;
    }
    else if(Controller_Manager::poll_event(event_storage)){
        return true;
    }
    else{
        return false;
    }
}

void Engine_Interface::update_window_caption(int render_rate,double ms_per_frame,int logic_frame_rate){
    string msg="";

    //Set the window caption.
    if(Options::dev){
        msg=Engine_Data::game_title+" (DEV Mode)";
    }
    else{
        msg=Engine_Data::game_title;
    }

    msg+=get_game_window_caption();

    if(Options::fps){
        msg+="  FPS: "+Strings::num_to_string(render_rate);

        msg+="  LUPS: "+Strings::num_to_string(logic_frame_rate);

        msg+="  MS/Frame: "+Strings::num_to_string(ms_per_frame);
    }

    Game_Window::set_title(msg);
}

void Engine_Interface::add_chat(string message){
    chat.add_text(message);
}

bool Engine_Interface::is_console_selected(){
    if(Engine::mutable_info_selected() && Engine::mutable_info_this(&console.info_input)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::is_chat_selected(){
    if(Engine::mutable_info_selected() && Engine::mutable_info_this(&chat.info_input)){
        return true;
    }
    else{
        return false;
    }
}

void Engine_Interface::input_backspace(){
    if(Engine::mutable_info_selected()){
        Engine::ptr_mutable_info->input_backspace();
    }
}

void Engine_Interface::input_delete(){
    if(Engine::mutable_info_selected()){
        Engine::ptr_mutable_info->input_delete();
    }
}

void Engine_Interface::input_newline(){
    if(Engine::mutable_info_selected()){
        Engine::ptr_mutable_info->input_newline();
    }
}

void Engine_Interface::prepare_for_input(){
    //Start each frame of updates with the tooltip off
    Tooltip::on=false;

    Engine::mouse_over=false;

    if(counter_gui_scroll_axis>0){
        counter_gui_scroll_axis--;
    }

    if(Engine::allow_screen_keyboard() && !Game_Window::is_screen_keyboard_shown() && Engine::mutable_info_selected()){
        Engine::clear_mutable_info();
    }

    Window_Manager::prepare_for_input();
}

bool Engine_Interface::handle_input_events_drag_and_drop(){
    bool event_consumed=false;

    switch(Engine::event.type){
    case SDL_DROPFILE:
        if(!event_consumed){
            string file=Engine::event.drop.file;
            SDL_free(Engine::event.drop.file);

            handle_drag_and_drop(file);

            event_consumed=true;
        }
        break;
    }

    return event_consumed;
}

bool Engine_Interface::handle_input_events_touch(){
    bool event_consumed=false;

    switch(Engine::event.type){
    case SDL_FINGERDOWN:
        if(!event_consumed){
            Controller_Manager::finger_down(Engine::event);

            event_consumed=true;
        }
        break;
    }

    return event_consumed;
}

bool Engine_Interface::handle_input_events(bool event_ignore_command_set){
    bool event_consumed=false;

    const uint8_t* keystates=SDL_GetKeyboardState(NULL);

    switch(Engine::event.type){
        case SDL_CONTROLLERDEVICEADDED:
            if(!event_consumed){
                Controller_Manager::add_controller(Engine::event);

                event_consumed=true;
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if(!event_consumed){
                Controller_Manager::remove_controller(Engine::event);

                event_consumed=true;
            }
            break;

        case SDL_TEXTINPUT:
            if(Engine::mutable_info_selected()){
                if(!event_consumed){
                    string text=Engine::event.text.text;

                    Engine::handle_text_input(text);

                    event_consumed=true;
                }
            }
            break;

        case SDL_CONTROLLERAXISMOTION:
            if(!event_ignore_command_set){
                //GUI nav controller axis up
                if(!event_consumed && Window_Manager::is_any_window_open() && !console.on && !Engine::mutable_info_selected() && gui_axis_nav_last_direction!="up" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTY && Engine::event.caxis.value<-Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="up";

                    gui_nav_up("controller");

                    event_consumed=true;
                }
                //GUI nav controller axis down
                if(!event_consumed && Window_Manager::is_any_window_open() && !console.on && !Engine::mutable_info_selected() && gui_axis_nav_last_direction!="down" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTY && Engine::event.caxis.value>Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="down";

                    gui_nav_down("controller");

                    event_consumed=true;
                }
                //GUI nav controller axis left
                if(!event_consumed && Engine::mutable_info_selected() && gui_axis_nav_last_direction!="left" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTX && Engine::event.caxis.value<-Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="left";

                    gui_nav_left();

                    event_consumed=true;
                }
                //GUI nav controller axis right
                if(!event_consumed && Engine::mutable_info_selected() && gui_axis_nav_last_direction!="right" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTX && Engine::event.caxis.value>Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="right";

                    gui_nav_right();

                    event_consumed=true;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            if(!event_ignore_command_set){
                if(Engine::mutable_info_selected() && !Engine::allow_screen_keyboard()){
                    if(!Engine_Data::controller_text_entry_small){
                        if(!event_consumed && Engine::ptr_mutable_info->allows_input("newline") && Engine::ptr_mutable_info->text.length()<Engine::ptr_mutable_info->max_text_length &&
                           Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSTICK){
                            input_newline();

                            event_consumed=true;
                        }

                        if(!event_consumed && is_console_selected() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSTICK){
                            console.do_command();

                            event_consumed=true;
                        }
                        else if(!event_consumed && is_chat_selected() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSTICK){
                            chat.send_chat();

                            event_consumed=true;
                        }

                        if(!event_consumed && Engine::ptr_mutable_info->allows_input("backspace") && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSHOULDER && Engine::ptr_mutable_info->text.length()>0){
                            input_backspace();

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_RIGHTSHOULDER){
                            Engine::handle_text_input(" ");

                            event_consumed=true;
                        }
                        if(!event_consumed && (Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_A || Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_B ||
                                               Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_X || Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_Y)){
                            vector<string>* characters=Engine::get_text_input_character_set();
                            int selected_chunk=Engine::get_text_input_selected_chunk();

                            if(selected_chunk>=0 && selected_chunk<8){
                                int character_offset=0;
                                if(Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_X){
                                    character_offset=0;
                                }
                                else if(Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_Y){
                                    character_offset=1;
                                }
                                else if(Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                                    character_offset=2;
                                }
                                else if(Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                                    character_offset=3;
                                }

                                Engine::handle_text_input(characters->at((selected_chunk*4)+character_offset));

                                event_consumed=true;
                            }
                        }
                    }
                    else{
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_LEFT){
                            if(--text_entry_small_selector.x<0){
                                text_entry_small_selector.x=12;
                            }

                            Sound_Manager::play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT){
                            if(++text_entry_small_selector.x>=13){
                                text_entry_small_selector.x=0;
                            }

                            Sound_Manager::play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                            if(--text_entry_small_selector.y<0){
                                text_entry_small_selector.y=7;
                            }

                            Sound_Manager::play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                            if(++text_entry_small_selector.y>=8){
                                text_entry_small_selector.y=0;
                            }

                            Sound_Manager::play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                            if(Engine::ptr_mutable_info->allows_input("backspace") && Engine::ptr_mutable_info->text.length()>0){
                                input_backspace();
                            }
                            else{
                                gui_nav_back_controller();
                            }

                            Sound_Manager::play_sound("text_input_small_select");

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_START){
                            text_entry_small_selector.x=12;
                            text_entry_small_selector.y=7;

                            Sound_Manager::play_sound("text_input_small_move");

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                            string text_entry_char=Engine::get_text_entry_small_character();

                            if((int)text_entry_char[0]==1){
                                if(Engine::ptr_mutable_info->allows_input("backspace") && Engine::ptr_mutable_info->text.length()>0){
                                    input_backspace();
                                }
                            }
                            else if((int)text_entry_char[0]==2){
                                if(is_console_selected()){
                                    console.do_command();
                                }
                                else if(is_chat_selected()){
                                    chat.send_chat();
                                }
                                else{
                                    Engine::clear_mutable_info();
                                }
                            }
                            else{
                                Engine::handle_text_input(text_entry_char);
                            }

                            Sound_Manager::play_sound("text_input_small_select");

                            event_consumed=true;
                        }
                    }
                }

                //GUI nav controller back
                if(!event_consumed && (Engine::mutable_info_selected() || Window_Manager::is_any_window_open()) && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                    gui_nav_back_controller();

                    event_consumed=true;
                }
                //GUI nav controller toggle menu
                if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_START){
                    gui_nav_toggle_menu_controller();

                    event_consumed=true;
                }
                //GUI nav controller confirm
                if(!event_consumed && !Engine::mutable_info_selected() && Window_Manager::is_any_window_open() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                    gui_nav_confirm("controller");

                    event_consumed=true;
                }
                //GUI nav controller up
                if(!event_consumed && Window_Manager::is_any_window_open() && !console.on && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                    gui_nav_up("controller");

                    event_consumed=true;
                }
                //GUI nav controller down
                if(!event_consumed && Window_Manager::is_any_window_open() && !console.on && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                    gui_nav_down("controller");

                    event_consumed=true;
                }
                //GUI nav controller left
                if(!event_consumed && Engine::mutable_info_selected() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_LEFT){
                    gui_nav_left();

                    event_consumed=true;
                }
                //GUI nav controller right
                if(!event_consumed && Engine::mutable_info_selected() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT){
                    gui_nav_right();

                    event_consumed=true;
                }
            }
            break;

        case SDL_KEYDOWN:
            if(!event_ignore_command_set){
                if(Engine::mutable_info_selected()){
                    if(!event_consumed && Engine::ptr_mutable_info->allows_input("newline") && Engine::ptr_mutable_info->text.length()<Engine::ptr_mutable_info->max_text_length &&
                       (Engine::event.key.keysym.scancode==SDL_SCANCODE_RETURN || Engine::event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        input_newline();

                        event_consumed=true;
                    }

                    if(!event_consumed && Engine::event.key.repeat==0 && is_console_selected() && (Engine::event.key.keysym.scancode==SDL_SCANCODE_RETURN || Engine::event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        console.do_command();

                        event_consumed=true;
                    }
                    else if(!event_consumed && Engine::event.key.repeat==0 && is_chat_selected() && (Engine::event.key.keysym.scancode==SDL_SCANCODE_RETURN || Engine::event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        chat.send_chat();

                        event_consumed=true;
                    }

                    if(!event_consumed && Engine::ptr_mutable_info->allows_input("backspace") && Engine::event.key.keysym.scancode==SDL_SCANCODE_BACKSPACE && Engine::ptr_mutable_info->text.length()>0){
                        input_backspace();

                        event_consumed=true;
                    }

                    if(!event_consumed && Engine::ptr_mutable_info->allows_input("backspace") && Engine::event.key.keysym.scancode==SDL_SCANCODE_DELETE && Engine::ptr_mutable_info->text.length()>0){
                        input_delete();

                        event_consumed=true;
                    }

                    if(!event_consumed && !is_console_selected() && Engine::ptr_mutable_info->allows_input("space") && Engine::event.key.keysym.scancode==SDL_SCANCODE_TAB){
                        Engine::handle_text_input("\t");

                        event_consumed=true;
                    }
                }

                if(Engine::event.key.repeat==0){
                    //Screenshot
                    if(!event_consumed && Engine::event.key.keysym.scancode==SDL_SCANCODE_F5){
                        Game_Window::screenshot();

                        event_consumed=true;
                    }

                    //Toggle fullscreen
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && (Engine::event.key.keysym.scancode==SDL_SCANCODE_RETURN || Engine::event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        Options::change_option("cl_fullscreen_state",Strings::bool_to_string(!Options::fullscreen));

                        Engine_Mailman::send_letter("reload");

                        event_consumed=true;
                    }

                    //Quit game
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && Engine::event.key.keysym.scancode==SDL_SCANCODE_F4){
                        Engine::quit();

                        event_consumed=true;
                    }

                    //Toggle GUI display
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && Engine::event.key.keysym.scancode==SDL_SCANCODE_Z){
                        hide_gui=!hide_gui;

                        event_consumed=true;
                    }

                    //GUI nav Android back
                    if(!event_consumed && Engine::event.key.keysym.scancode==SDL_SCANCODE_AC_BACK){
                        gui_nav_back_android();

                        event_consumed=true;
                    }
                    //GUI nav Android toggle menu
                    if(!event_consumed && Engine::event.key.keysym.scancode==SDL_SCANCODE_MENU){
                        gui_nav_toggle_menu_android();

                        event_consumed=true;
                    }
                    //GUI nav keyboard back
                    if(!event_consumed && Engine::event.key.keysym.scancode==SDL_SCANCODE_ESCAPE){
                        gui_nav_back_keyboard_and_mouse();

                        event_consumed=true;
                    }
                    //GUI nav keyboard confirm
                    if(!event_consumed && !Engine::mutable_info_selected() && Window_Manager::is_any_window_open() && (Engine::event.key.keysym.scancode==SDL_SCANCODE_RETURN || Engine::event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        gui_nav_confirm("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard up
                    if(!event_consumed && Window_Manager::is_any_window_open() && !console.on && Engine::event.key.keysym.scancode==SDL_SCANCODE_UP){
                        gui_nav_up("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard down
                    if(!event_consumed && Window_Manager::is_any_window_open() && !console.on && Engine::event.key.keysym.scancode==SDL_SCANCODE_DOWN){
                        gui_nav_down("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard left
                    if(!event_consumed && Engine::mutable_info_selected() && Engine::event.key.keysym.scancode==SDL_SCANCODE_LEFT){
                        gui_nav_left();

                        event_consumed=true;
                    }
                    //GUI nav keyboard right
                    if(!event_consumed && Engine::mutable_info_selected() && Engine::event.key.keysym.scancode==SDL_SCANCODE_RIGHT){
                        gui_nav_right();

                        event_consumed=true;
                    }
                    //GUI nav keyboard scroll up
                    if(!event_consumed && (Window_Manager::is_any_window_open() || console.on || chat.on) && Engine::event.key.keysym.scancode==SDL_SCANCODE_PAGEUP){
                        gui_scroll_up("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard scroll down
                    if(!event_consumed && (Window_Manager::is_any_window_open() || console.on || chat.on) && Engine::event.key.keysym.scancode==SDL_SCANCODE_PAGEDOWN){
                        gui_scroll_down("keyboard");

                        event_consumed=true;
                    }
                }

                //Paste
                if(!event_consumed && (keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL]) && Engine::event.key.keysym.scancode==SDL_SCANCODE_V){
					if(SDL_HasClipboardText() && Engine::mutable_info_selected()){
                        char* text=SDL_GetClipboardText();
                        string str_text=text;
                        SDL_free(text);

                        vector<string> lines;
                        boost::algorithm::split(lines,str_text,boost::algorithm::is_any_of("\n"));

                        for(int i=0;i<lines.size();i++){
                            Engine::handle_text_input(lines[i]);

                            if(i<lines.size()-1 && Engine::ptr_mutable_info->allows_input("newline") && Engine::ptr_mutable_info->text.length()<Engine::ptr_mutable_info->max_text_length){
                                input_newline();
                            }
                        }
                    }

                    event_consumed=true;
                }
            }
            break;

        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: case SDL_MOUSEWHEEL:
            //GUI nav mouse
            if(!event_consumed && Engine::mouse_allowed()){
                set_gui_mode("mouse");
            }
            break;

        case SDL_WINDOWEVENT:
            if(!event_consumed){
                if(Engine::event.window.event==SDL_WINDOWEVENT_MOVED){
                    Game_Window::update_display_number();

                    event_consumed=true;
                }
            }
            break;
    }

    if(!console.on && !event_ignore_command_set && Engine::mouse_allowed()){
        if(Window_Manager::window_under_mouse!=0){
            if(!event_consumed){
                event_consumed=Window_Manager::window_under_mouse->handle_input_events();
            }
        }

        if(!event_consumed){
            event_consumed=chat.handle_input_events();
        }
    }

    if(!event_consumed && !event_ignore_command_set){
        event_consumed=console.handle_input_events();
    }

    return event_consumed;
}

void Engine_Interface::handle_input_states(){
    if(!console.on){
        Window* top_window=Window_Manager::get_top_window();

        for(int i=0;i<windows.size();i++){
            if(top_window==0 || top_window!=&windows[i]){
                windows[i].reset_buttons_moused_over();
            }
        }

        if(top_window!=0){
            top_window->handle_input_states();
        }

        chat.handle_input_states();
    }

    int axis_left_x=Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_LEFTX);
    int axis_left_y=Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_LEFTY);
    if(axis_left_x>=-Engine_Data::controller_dead_zone && axis_left_x<=Engine_Data::controller_dead_zone && axis_left_y>=-Engine_Data::controller_dead_zone && axis_left_y<=Engine_Data::controller_dead_zone){
        gui_axis_nav_last_direction="none";
    }

    if(Window_Manager::is_any_window_open() || console.on || chat.on){
        int axis_right_y=Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_RIGHTY);
        if(counter_gui_scroll_axis==0 && axis_right_y<-Engine_Data::controller_dead_zone){
            counter_gui_scroll_axis=Engine_Data::axis_scroll_rate;

            gui_scroll_up("controller");
        }
        else if(counter_gui_scroll_axis==0 && axis_right_y>Engine_Data::controller_dead_zone){
            counter_gui_scroll_axis=Engine_Data::axis_scroll_rate;

            gui_scroll_down("controller");
        }
    }

    console.handle_input_states();
}

string Engine_Interface::get_system_info(){
    string msg="";

    int logical_width=0;
    int logical_height=0;
    Game_Window::get_renderer_logical_size(&logical_width,&logical_height);

    SDL_Rect rect;
    Game_Window::get_renderer_viewport(&rect);

    float scale_x=0.0;
    float scale_y=0.0;
    Game_Window::get_renderer_scale(&scale_x,&scale_y);

    int actual_width=0;
    int actual_height=0;
    Game_Window::get_renderer_output_size(&actual_width,&actual_height);

    SDL_RendererInfo info;
    Game_Window::get_renderer_info(&info);
    string renderer_name=info.name;

    int mouse_x=0;
    int mouse_y=0;
    Engine::get_mouse_state(&mouse_x,&mouse_y);

    int mouse_real_x=0;
    int mouse_real_y=0;
    SDL_GetMouseState(&mouse_real_x,&mouse_real_y);

    int power_seconds=0;
    int power_percentage=0;
    SDL_PowerState power_state=SDL_GetPowerInfo(&power_seconds,&power_percentage);
    string str_power_seconds=Strings::time_string(power_seconds)+" remaining";
    string str_power_percentage=Strings::num_to_string(power_percentage)+"%";

    Controller_Manager::get_controller_info(msg);

    msg+="Resolution (Logical): "+Strings::num_to_string(logical_width)+" x "+Strings::num_to_string(logical_height)+"\n";
    msg+="Logical Viewport: "+Strings::num_to_string(rect.x)+","+Strings::num_to_string(rect.y)+","+Strings::num_to_string(rect.w)+","+Strings::num_to_string(rect.h)+"\n";
    msg+="Render Scale: "+Strings::num_to_string(scale_x)+","+Strings::num_to_string(scale_y)+"\n";
    msg+="Resolution (Actual): "+Strings::num_to_string(actual_width)+" x "+Strings::num_to_string(actual_height)+"\n";
    msg+="Renderer: "+renderer_name+"\n";
    msg+="Max Texture Size: "+Strings::num_to_string(info.max_texture_width)+" x "+Strings::num_to_string(info.max_texture_height)+"\n";
    msg+="Current Gui Mode: "+Strings::first_letter_capital(Engine::gui_mode)+"\n";

    msg+="Mouse Position (Logical): "+Strings::num_to_string(mouse_x)+","+Strings::num_to_string(mouse_y)+"\n";
    msg+="Mouse Position (Actual): "+Strings::num_to_string(mouse_real_x)+","+Strings::num_to_string(mouse_real_y)+"\n";

    msg+="Power State: ";
    if(power_state==SDL_POWERSTATE_UNKNOWN){
        msg+="Unknown";
    }
    else if(power_state==SDL_POWERSTATE_ON_BATTERY){
        msg+="Unplugged, on battery";
    }
    else if(power_state==SDL_POWERSTATE_NO_BATTERY){
        msg+="Plugged in, no battery";
    }
    else if(power_state==SDL_POWERSTATE_CHARGING){
        msg+="Plugged in, charging";
    }
    else if(power_state==SDL_POWERSTATE_CHARGED){
        msg+="Plugged in, fully charged";
    }
    if(power_seconds!=-1 || power_percentage!=-1){
        msg+=" (";
    }
    if(power_percentage!=-1){
        msg+=str_power_percentage;
    }
    if(power_seconds!=-1){
        if(power_percentage!=-1){
            msg+=", ";
        }
        msg+=str_power_seconds;
    }
    if(power_seconds!=-1 || power_percentage!=-1){
        msg+=")";
    }

    return msg;
}

void Engine_Interface::animate(){
    Window_Manager::animate();

    console.animate();
    chat.animate();

    Engine::animate();

    Object_Manager::animate_cursors();

    GUI_Manager::animate();
}

void Engine_Interface::render(int render_rate,double ms_per_frame,int logic_frame_rate){
    if(!hide_gui){
        if(Game_Manager::in_progress && Game_Manager::paused){
            render_pause();
        }

        Window_Manager::render();

        GUI_Manager::render_gui_selector();

        console.render();
        chat.render();

        Tooltip::render();

        Engine::render_toast();

        if(Engine::mutable_info_selected() && Engine::gui_mode=="controller" && !Engine::allow_screen_keyboard()){
            if(!Engine_Data::controller_text_entry_small){
                Engine::render_text_inputter();
            }
            else{
                Engine::render_small_text_inputter();
            }
        }

        if(Engine::mutable_info_selected() && Engine::allow_screen_keyboard() && Game_Window::is_screen_keyboard_shown()){
            Engine::render_text_editing();
        }

        Controller_Manager::render_touch_controller();

        if(Options::dev){
            render_dev_info();
        }

        if(Options::fps){
            render_fps(render_rate,ms_per_frame,logic_frame_rate);
        }

        if(Engine::gui_mode=="mouse" && (Engine_Data::cursor_render_always || Window_Manager::is_any_window_open() || console.on)){
            int mouse_x=0;
            int mouse_y=0;
            Engine::get_mouse_state(&mouse_x,&mouse_y);

            if(!Engine::mouse_over){
                Object_Manager::get_cursor(Engine_Data::cursor)->render(mouse_x,mouse_y);
            }
            else{
                Object_Manager::get_cursor(Engine_Data::cursor_mouse_over)->render(mouse_x,mouse_y);
            }
        }
        else{
            if(Options::hw_cursor){
                SDL_ShowCursor(SDL_DISABLE);
            }
        }
    }
    else{
        if(Options::hw_cursor){
            SDL_ShowCursor(SDL_DISABLE);
        }
    }
}
