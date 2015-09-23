/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "engine_interface.h"
#include "world.h"
#include "game_options.h"

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

GUI_Object::GUI_Object(string get_type,int get_index,int get_x,int get_y){
    type=get_type;
    index=get_index;
    x=get_x;
    y=get_y;
    sort_by_y=false;
}

bool GUI_Object::operator<=(GUI_Object object){
    if(sort_by_y){
        return y<=object.y;
    }
    else{
        return x<=object.x;
    }
}

void GUI_Object::set_sort_by_y(vector<GUI_Object>& objects,bool sort_value){
    for(int i=0;i<objects.size();i++){
        objects[i].sort_by_y=sort_value;
    }
}

GUI_Selector_Chaser::GUI_Selector_Chaser(){
    x=-1.0;
    y=-1.0;
}

Engine_Interface::Engine_Interface(){
    window_under_mouse=0;

    hide_gui=false;

    mouse_over=false;

    gui_mode="mouse";
    gui_selected_object=-1;
    gui_selector_style="";
    for(int i=0;i<2;i++){
        gui_selector_chasers.push_back(GUI_Selector_Chaser());
    }

    gui_axis_nav_last_direction="none";

    counter_gui_scroll_axis=0;

    configure_command=-1;

    editing_server=0;

    counter_cursor=0;
    cursor_fade_direction=false;
    cursor_opacity=10;

    ptr_mutable_info=0;
}

void Engine_Interface::quit(){
    Game_Manager::stop();

    unload_world();

    Game_Window::deinitialize();

    exit(EXIT_SUCCESS);
}

void Engine_Interface::build_text_input_characters(){
    characters_symbols.clear();

    characters_lower.clear();
    for(char i='a';i<='z';i++){
        characters_lower.push_back(string(1,i));
    }

    if(!Engine_Data::controller_text_entry_small){
        characters_lower.push_back(",");
        characters_lower.push_back(".");
        characters_lower.push_back(":");
        characters_lower.push_back("/");
        characters_lower.push_back("@");
        characters_lower.push_back("-");
    }
    else{
        characters_symbols.push_back(",");
        characters_symbols.push_back(".");
        characters_symbols.push_back(":");
        characters_symbols.push_back("/");
        characters_symbols.push_back("@");
        characters_symbols.push_back("-");
    }

    characters_upper.clear();
    for(char i='A';i<='Z';i++){
        characters_upper.push_back(string(1,i));
    }

    if(!Engine_Data::controller_text_entry_small){
        characters_upper.push_back("?");
        characters_upper.push_back("!");
        characters_upper.push_back(";");
        characters_upper.push_back("\\");
        characters_upper.push_back("&");
        characters_upper.push_back("_");
    }
    else{
        characters_symbols.push_back("?");
        characters_symbols.push_back("!");
        characters_symbols.push_back(";");
        characters_symbols.push_back("\\");
        characters_symbols.push_back("&");
        characters_symbols.push_back("_");
    }

    characters_numbers.clear();
    characters_numbers.push_back("1");
    characters_numbers.push_back("2");
    characters_numbers.push_back("3");
    characters_numbers.push_back("4");
    characters_numbers.push_back("5");
    characters_numbers.push_back("6");
    characters_numbers.push_back("7");
    characters_numbers.push_back("8");
    characters_numbers.push_back("9");
    characters_numbers.push_back("0");

    if(!Engine_Data::controller_text_entry_small){
        characters_numbers.push_back("*");
        characters_numbers.push_back("+");
        characters_numbers.push_back(string(1,(unsigned char)156));
        characters_numbers.push_back(string(1,(unsigned char)155));
        characters_numbers.push_back("$");
        characters_numbers.push_back("`");
        characters_numbers.push_back("'");
        characters_numbers.push_back("\"");
        characters_numbers.push_back("~");
        characters_numbers.push_back("|");
        characters_numbers.push_back("=");
        characters_numbers.push_back("#");
        characters_numbers.push_back("%");
        characters_numbers.push_back("^");
        characters_numbers.push_back("<");
        characters_numbers.push_back(">");
        characters_numbers.push_back("[");
        characters_numbers.push_back("]");
        characters_numbers.push_back("{");
        characters_numbers.push_back("}");
        characters_numbers.push_back("(");
        characters_numbers.push_back(")");
    }
    else{
        characters_numbers.push_back(" ");
        characters_numbers.push_back(" ");
        characters_numbers.push_back(" ");

        characters_symbols.push_back("*");
        characters_symbols.push_back("+");
        characters_symbols.push_back(string(1,(unsigned char)156));
        characters_symbols.push_back(string(1,(unsigned char)155));
        characters_symbols.push_back("$");
        characters_symbols.push_back("`");
        characters_symbols.push_back("'");
        characters_symbols.push_back("\"");
        characters_symbols.push_back("~");
        characters_symbols.push_back("|");
        characters_symbols.push_back("=");
        characters_symbols.push_back("#");
        characters_symbols.push_back("%");
        characters_symbols.push_back("^");
        characters_symbols.push_back("<");
        characters_symbols.push_back(">");
        characters_symbols.push_back("[");
        characters_symbols.push_back("]");
        characters_symbols.push_back("{");
        characters_symbols.push_back("}");
        characters_symbols.push_back("(");
        characters_symbols.push_back(")");
        characters_symbols.push_back(" ");
        characters_symbols.push_back(" ");
        characters_symbols.push_back(" ");
        characters_symbols.push_back(string(1,(unsigned char)1));
        characters_symbols.push_back(string(1,(unsigned char)2));
    }
}

void Engine_Interface::set_logic_update_rate(double frame_rate){
    Engine::UPDATE_RATE=frame_rate;
    Engine::SKIP_TICKS=1000.0/Engine::UPDATE_RATE;
}

void Engine_Interface::set_render_update_rate(double frame_rate){
    Engine::UPDATE_RATE_RENDER=frame_rate;
    Engine::SKIP_TICKS_RENDER=1000.0/Engine::UPDATE_RATE_RENDER;
}

bool Engine_Interface::load_data_engine(){
    bool load_result=load_data("engine");

    build_text_input_characters();

    return load_result;
}

void Engine_Interface::load_data_main(){
    load_data("font");
    load_data("cursor");
    load_data("color");
    load_data("color_theme");
    load_data("animation");
    load_data("window");
    load_data("game_command");
    load_data("game_constant");
    load_data("custom_sound");

    load_data_game();

    for(int i=0;i<windows.size();i++){
        windows[i].create_close_button();
        windows[i].set_last_normal_button();
    }

    Object_Manager::load_hw_cursors();

    text_selector.set_name("text_selector");
}

void Engine_Interface::load_data_game_options(){
    load_data("game_option");
}

bool Engine_Interface::load_data(string tag){
    //Look through all of the files in the directory.
    for(File_IO_Directory_Iterator it("data");it.evaluate();it.iterate()){
        //If the file is not a directory.
        if(it.is_regular_file()){
            string file_path=it.get_full_path();

            File_IO_Load load(file_path);

            if(load.is_opened()){
                bool multi_line_comment=false;

                //As long as we haven't reached the end of the file.
                while(!load.eof()){
                    string line="";

                    //Grab the next line of the file.
                    load.getline(&line);

                    //Clear initial whitespace from the line.
                    boost::algorithm::trim(line);

                    //If the line ends a multi-line comment.
                    if(boost::algorithm::contains(line,"*/")){
                        multi_line_comment=false;
                    }
                    //If the line starts a multi-line comment.
                    if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
                        multi_line_comment=true;
                    }
                    //If the line is a comment.
                    else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
                        //Ignore this line.
                    }

                    //If the line begins an instance of the passed tag type.
                    else if(!multi_line_comment && boost::algorithm::starts_with(line,"<"+tag+">")){
                        if(tag=="engine"){
                            load_engine_data(&load);
                        }
                        else if(tag=="font"){
                            Object_Manager::load_font(&load);
                        }
                        else if(tag=="cursor"){
                            Object_Manager::load_cursor(&load);
                        }
                        else if(tag=="color"){
                            Object_Manager::load_color(&load);
                        }
                        else if(tag=="color_theme"){
                            Object_Manager::load_color_theme(&load);
                        }
                        else if(tag=="animation"){
                            Object_Manager::load_animation(&load);
                        }
                        else if(tag=="window"){
                            load_window(&load);
                        }
                        else if(tag=="game_command"){
                            Object_Manager::load_game_command(&load);
                        }
                        else if(tag=="game_option"){
                            Object_Manager::load_game_option(&load);
                        }
                        else if(tag=="game_constant"){
                            Object_Manager::load_game_constant(&load);
                        }
                        else if(tag=="custom_sound"){
                            Object_Manager::load_custom_sound(&load);
                        }
                        else{
                            load_data_tag_game(tag,&load);
                        }
                    }
                }
            }
            else{
                Log::add_error("Error loading tag data: '"+tag+"'");

                return false;
            }
        }
    }

    return true;
}

void Engine_Interface::unload_data(){
    clear_mutable_info();

    Object_Manager::unload_data();

    windows.clear();

    window_z_order.clear();
    window_under_mouse=0;

    unload_data_game();
}

void Engine_Interface::load_engine_data(File_IO_Load* load){
    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_home_directory="home_directory:";
        string str_developer="developer:";
        string str_starting_windows="starting_windows:";
        string str_logical_screen_width="logical_screen_width:";
        string str_logical_screen_height="logical_screen_height:";
        string str_resolution_mode="resolution_mode:";
        string str_frame_rate="logic_update_rate:";
        string str_frame_rate_render="frame_rate_max:";
        string str_axis_scroll_rate="axis_scroll_rate:";
        string str_scrolling_buttons_offset="scrolling_buttons_offset:";
        string str_cursor_width="cursor_width:";
        string str_cursor_height="cursor_height:";
        string str_console_height="console_height:";
        string str_chat_height="chat_height:";
        string str_sound_falloff="sound_falloff:";
        string str_window_border_thickness="window_border_thickness:";
        string str_gui_border_thickness="gui_border_thickness:";
        string str_drag_and_drop="drag_and_drop:";
        string str_touch_finger_size="touch_finger_size:";
        string str_touch_controller_shoulders="touch_controller_shoulders:";
        string str_touch_controller_guide="touch_controller_guide:";
        string str_touch_controller_xy="touch_controller_xy:";
        string str_tooltip_font="tooltip_font:";
        string str_toast_font="toast_font:";
        string str_default_font="default_font:";
        string str_toast_length_short="toast_length_short:";
        string str_toast_length_medium="toast_length_medium:";
        string str_toast_length_long="toast_length_long:";
        string str_spaces_per_tab="spaces_per_tab:";
        string str_console_move_speed="console_move_speed:";
        string str_console_max_command_length="console_max_command_length:";
        string str_console_max_log_recall="console_max_log_recall:";
        string str_console_max_command_recall="console_max_command_recall:";
        string str_console_font="console_font:";
        string str_console_font_color="console_font_color:";
        string str_console_opacity="console_opacity:";
        string str_chat_move_speed="chat_move_speed:";
        string str_chat_max_command_length="chat_max_command_length:";
        string str_chat_max_log_recall="chat_max_log_recall:";
        string str_chat_max_command_recall="chat_max_command_recall:";
        string str_chat_font="chat_font:";
        string str_chat_font_color="chat_font_color:";
        string str_chat_opacity="chat_opacity:";
        string str_chat_line_timeout="chat_line_timeout:";
        string str_gui_selector_style="gui_selector_style:";
        string str_controller_dead_zone="controller_dead_zone:";
        string str_color_theme="color_theme:";
        string str_controller_text_entry_small="controller_text_entry_small:";
        string str_cursor_render_always="cursor_render_always:";
        string str_cursor="cursor:";
        string str_cursor_mouse_over="cursor_mouse_over:";
        string str_default_save_location="default_save_location:";

        string str_default_screen_width="default_screen_width:";
        string str_default_screen_height="default_screen_height:";
        string str_default_fullscreen="default_fullscreen_state:";
        string str_default_fullscreen_mode="default_fullscreen_mode:";

        string str_default_vsync="default_vsync:";
        string str_default_accelerometer_controller="default_accelerometer_controller:";
        string str_default_touch_controller_state="default_touch_controller_state:";
        string str_default_touch_controller_opacity="default_touch_controller_opacity:";
        string str_default_font_shadows="default_font_shadows:";
        string str_default_screen_keyboard="default_screen_keyboard:";
        string str_default_hw_cursor="default_hw_cursor:";
        string str_default_bind_cursor="default_bind_cursor:";

        string str_default_fps="default_fps:";
        string str_default_dev="default_dev:";
        string str_default_volume_global="default_volume_global:";
        string str_default_volume_sound="default_volume_sound:";
        string str_default_volume_music="default_volume_music:";
        string str_default_mute_global="default_mute_global:";
        string str_default_mute_sound="default_mute_sound:";
        string str_default_mute_music="default_mute_music:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            Engine_Data::game_title=line;
        }
        //Home directory
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_home_directory)){
            //Clear the data name.
            line.erase(0,str_home_directory.length());

            Directories::home_directory=line;
        }
        //Developer
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_developer)){
            //Clear the data name.
            line.erase(0,str_developer.length());

            Engine_Data::developer=line;
        }
        //Starting windows
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_starting_windows)){
            //Clear the data name.
            line.erase(0,str_starting_windows.length());

            Engine_Data::starting_windows.clear();
            boost::algorithm::split(Engine_Data::starting_windows,line,boost::algorithm::is_any_of(","));

            for(int i=0;i<Engine_Data::starting_windows.size();i++){
                if(Engine_Data::starting_windows[i].length()==0){
                    Engine_Data::starting_windows.erase(Engine_Data::starting_windows.begin()+i);
                    i--;
                }
            }
        }
        //Logical screen width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_logical_screen_width)){
            //Clear the data name.
            line.erase(0,str_logical_screen_width.length());

            Engine_Data::logical_screen_width=Strings::string_to_long(line);
        }
        //Logical screen height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_logical_screen_height)){
            //Clear the data name.
            line.erase(0,str_logical_screen_height.length());

            Engine_Data::logical_screen_height=Strings::string_to_long(line);
        }
        //Resolution mode
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_resolution_mode)){
            //Clear the data name.
            line.erase(0,str_resolution_mode.length());

            Engine_Data::resolution_mode=line;
        }
        //Frame rate
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_frame_rate)){
            //Clear the data name.
            line.erase(0,str_frame_rate.length());

            set_logic_update_rate(Strings::string_to_double(line));
        }
        //Frame rate render
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_frame_rate_render)){
            //Clear the data name.
            line.erase(0,str_frame_rate_render.length());

            set_render_update_rate(Strings::string_to_double(line));
        }
        //Axis scroll rate
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_axis_scroll_rate)){
            //Clear the data name.
            line.erase(0,str_axis_scroll_rate.length());

            Engine_Data::axis_scroll_rate=Strings::string_to_long(line);
        }
        //Scrolling buttons offset
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scrolling_buttons_offset)){
            //Clear the data name.
            line.erase(0,str_scrolling_buttons_offset.length());

            Engine_Data::scrolling_buttons_offset=Strings::string_to_long(line);
        }
        //Cursor width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_width)){
            //Clear the data name.
            line.erase(0,str_cursor_width.length());

            Engine_Data::cursor_width=Strings::string_to_long(line);
        }
        //Cursor height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_height)){
            //Clear the data name.
            line.erase(0,str_cursor_height.length());

            Engine_Data::cursor_height=Strings::string_to_long(line);
        }
        //Console height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_height)){
            //Clear the data name.
            line.erase(0,str_console_height.length());

            Engine_Data::console_height=Strings::string_to_long(line);
        }
        //Chat height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_height)){
            //Clear the data name.
            line.erase(0,str_chat_height.length());

            Engine_Data::chat_height=Strings::string_to_long(line);
        }
        //Sound falloff
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sound_falloff)){
            //Clear the data name.
            line.erase(0,str_sound_falloff.length());

            Engine_Data::sound_falloff=Strings::string_to_double(line);
        }
        //Window border thickness
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_window_border_thickness)){
            //Clear the data name.
            line.erase(0,str_window_border_thickness.length());

            Engine_Data::window_border_thickness=Strings::string_to_double(line);
        }
        //GUI border thickness
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_border_thickness)){
            //Clear the data name.
            line.erase(0,str_gui_border_thickness.length());

            Engine_Data::gui_border_thickness=Strings::string_to_double(line);
        }
        //Drag and drop
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_drag_and_drop)){
            //Clear the data name.
            line.erase(0,str_drag_and_drop.length());

            Engine_Data::drag_and_drop=Strings::string_to_bool(line);
        }
        //Touch finger size
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_finger_size)){
            //Clear the data name.
            line.erase(0,str_touch_finger_size.length());

            Engine_Data::touch_finger_size=Strings::string_to_double(line);
        }
        //Touch controller shoulders
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_shoulders)){
            //Clear the data name.
            line.erase(0,str_touch_controller_shoulders.length());

            Engine_Data::touch_controller_shoulders=Strings::string_to_bool(line);
        }
        //Touch controller guide
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_guide)){
            //Clear the data name.
            line.erase(0,str_touch_controller_guide.length());

            Engine_Data::touch_controller_guide=Strings::string_to_bool(line);
        }
        //Touch controller xy
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_xy)){
            //Clear the data name.
            line.erase(0,str_touch_controller_xy.length());

            Engine_Data::touch_controller_xy=Strings::string_to_bool(line);
        }
        //Tooltip font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_font)){
            //Clear the data name.
            line.erase(0,str_tooltip_font.length());

            tooltip.font=line;
        }
        //Toast font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_font)){
            //Clear the data name.
            line.erase(0,str_toast_font.length());

            Engine_Data::toast_font=line;
        }
        //Default font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_font)){
            //Clear the data name.
            line.erase(0,str_default_font.length());

            Engine_Data::default_font=line;
        }
        //Toast length short
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_length_short)){
            //Clear the data name.
            line.erase(0,str_toast_length_short.length());

            Engine_Data::toast_length_short=Strings::string_to_long(line);
        }
        //Toast length medium
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_length_medium)){
            //Clear the data name.
            line.erase(0,str_toast_length_medium.length());

            Engine_Data::toast_length_medium=Strings::string_to_long(line);
        }
        //Toast length long
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_toast_length_long)){
            //Clear the data name.
            line.erase(0,str_toast_length_long.length());

            Engine_Data::toast_length_long=Strings::string_to_long(line);
        }
        //Spaces per tab
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_spaces_per_tab)){
            //Clear the data name.
            line.erase(0,str_spaces_per_tab.length());

            Engine_Data::spaces_per_tab=Strings::string_to_long(line);
        }
        //Console move speed
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_move_speed)){
            //Clear the data name.
            line.erase(0,str_console_move_speed.length());

            console.move_speed=Strings::string_to_long(line);
        }
        //Console max command length
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_max_command_length)){
            //Clear the data name.
            line.erase(0,str_console_max_command_length.length());

            console.max_command_length=Strings::string_to_long(line);
        }
        //Console max log recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_max_log_recall)){
            //Clear the data name.
            line.erase(0,str_console_max_log_recall.length());

            console.max_log_recall=Strings::string_to_long(line);
        }
        //Console max command recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_max_command_recall)){
            //Clear the data name.
            line.erase(0,str_console_max_command_recall.length());

            console.max_command_recall=Strings::string_to_long(line);
        }
        //Console font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_font)){
            //Clear the data name.
            line.erase(0,str_console_font.length());

            console.font=line;
        }
        //Console font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_font_color)){
            //Clear the data name.
            line.erase(0,str_console_font_color.length());

            console.font_color=line;
        }
        //Console opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_console_opacity)){
            //Clear the data name.
            line.erase(0,str_console_opacity.length());

            console.background_opacity=Strings::string_to_double(line);
        }
        //Chat move speed
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_move_speed)){
            //Clear the data name.
            line.erase(0,str_chat_move_speed.length());

            chat.move_speed=Strings::string_to_long(line);
        }
        //Chat max command length
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_max_command_length)){
            //Clear the data name.
            line.erase(0,str_chat_max_command_length.length());

            chat.max_command_length=Strings::string_to_long(line);
        }
        //Chat max log recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_max_log_recall)){
            //Clear the data name.
            line.erase(0,str_chat_max_log_recall.length());

            chat.max_log_recall=Strings::string_to_long(line);
        }
        //Chat max command recall
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_max_command_recall)){
            //Clear the data name.
            line.erase(0,str_chat_max_command_recall.length());

            chat.max_command_recall=Strings::string_to_long(line);
        }
        //Chat font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_font)){
            //Clear the data name.
            line.erase(0,str_chat_font.length());

            chat.font=line;
        }
        //Chat font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_font_color)){
            //Clear the data name.
            line.erase(0,str_chat_font_color.length());

            chat.font_color=line;
        }
        //Chat opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_opacity)){
            //Clear the data name.
            line.erase(0,str_chat_opacity.length());

            chat.background_opacity=Strings::string_to_double(line);
        }
        //Chat line timeout
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_chat_line_timeout)){
            //Clear the data name.
            line.erase(0,str_chat_line_timeout.length());

            chat.line_timeout=Strings::string_to_long(line);
        }
        //GUI selector style
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_gui_selector_style)){
            //Clear the data name.
            line.erase(0,str_gui_selector_style.length());

            gui_selector_style=line;
        }
        //Controller dead zone
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_controller_dead_zone)){
            //Clear the data name.
            line.erase(0,str_controller_dead_zone.length());

            Engine_Data::controller_dead_zone=Strings::string_to_long(line);
        }
        //Color theme
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_color_theme)){
            //Clear the data name.
            line.erase(0,str_color_theme.length());

            Engine_Data::color_theme=line;
        }
        //Controller text entry small
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_controller_text_entry_small)){
            //Clear the data name.
            line.erase(0,str_controller_text_entry_small.length());

            Engine_Data::controller_text_entry_small=Strings::string_to_bool(line);
        }
        //Cursor render always
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_render_always)){
            //Clear the data name.
            line.erase(0,str_cursor_render_always.length());

            Engine_Data::cursor_render_always=Strings::string_to_bool(line);
        }
        //Cursor
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor)){
            //Clear the data name.
            line.erase(0,str_cursor.length());

            Engine_Data::cursor=line;
        }
        //Cursor Mouse Over
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_cursor_mouse_over)){
            //Clear the data name.
            line.erase(0,str_cursor_mouse_over.length());

            Engine_Data::cursor_mouse_over=line;
        }
        //default_save_location
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_save_location)){
            //Clear the data name.
            line.erase(0,str_default_save_location.length());

            Options::save_location=line;
        }

        //default_screen_width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_screen_width)){
            //Clear the data name.
            line.erase(0,str_default_screen_width.length());

            Options::screen_width=Strings::string_to_long(line);
        }
        //default_screen_height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_screen_height)){
            //Clear the data name.
            line.erase(0,str_default_screen_height.length());

            Options::screen_height=Strings::string_to_long(line);
        }
        //default_fullscreen
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_fullscreen)){
            //Clear the data name.
            line.erase(0,str_default_fullscreen.length());

            Options::fullscreen=Strings::string_to_bool(line);
        }
        //default_fullscreen_mode
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_fullscreen_mode)){
            //Clear the data name.
            line.erase(0,str_default_fullscreen_mode.length());

            Options::fullscreen_mode=line;
        }

        //default_vsync
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_vsync)){
            //Clear the data name.
            line.erase(0,str_default_vsync.length());

            Options::vsync=Strings::string_to_bool(line);
        }
        //default_accelerometer_controller
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_accelerometer_controller)){
            //Clear the data name.
            line.erase(0,str_default_accelerometer_controller.length());

            Options::accelerometer_controller=Strings::string_to_bool(line);
        }
        //default_touch_controller_state
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_touch_controller_state)){
            //Clear the data name.
            line.erase(0,str_default_touch_controller_state.length());

            Options::touch_controller_state=Strings::string_to_bool(line);
        }
        //default_touch_controller_opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_touch_controller_opacity)){
            //Clear the data name.
            line.erase(0,str_default_touch_controller_opacity.length());

            Options::touch_controller_opacity=Strings::string_to_double(line);
        }
        //default_font_shadows
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_font_shadows)){
            //Clear the data name.
            line.erase(0,str_default_font_shadows.length());

            Options::font_shadows=Strings::string_to_bool(line);
        }
        //default_screen_keyboard
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_screen_keyboard)){
            //Clear the data name.
            line.erase(0,str_default_screen_keyboard.length());

            Options::screen_keyboard=Strings::string_to_bool(line);
        }
        //default_hw_cursor
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_hw_cursor)){
            //Clear the data name.
            line.erase(0,str_default_hw_cursor.length());

            Options::hw_cursor=Strings::string_to_bool(line);
        }
        //default_bind_cursor
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_bind_cursor)){
            //Clear the data name.
            line.erase(0,str_default_bind_cursor.length());

            Options::bind_cursor=Strings::string_to_bool(line);
        }

        //default_fps
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_fps)){
            //Clear the data name.
            line.erase(0,str_default_fps.length());

            Options::fps=Strings::string_to_bool(line);
        }
        //default_dev
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_dev)){
            //Clear the data name.
            line.erase(0,str_default_dev.length());

            Options::dev=Strings::string_to_bool(line);
        }
        //default_volume_global
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_volume_global)){
            //Clear the data name.
            line.erase(0,str_default_volume_global.length());

            Options::volume_global=Strings::string_to_double(line);
        }
        //default_volume_sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_volume_sound)){
            //Clear the data name.
            line.erase(0,str_default_volume_sound.length());

            Options::volume_sound=Strings::string_to_double(line);
        }
        //default_volume_music
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_volume_music)){
            //Clear the data name.
            line.erase(0,str_default_volume_music.length());

            Options::volume_music=Strings::string_to_double(line);
        }
        //default_mute_global
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_mute_global)){
            //Clear the data name.
            line.erase(0,str_default_mute_global.length());

            Options::mute_global=Strings::string_to_bool(line);
        }
        //default_mute_sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_mute_sound)){
            //Clear the data name.
            line.erase(0,str_default_mute_sound.length());

            Options::mute_sound=Strings::string_to_bool(line);
        }
        //default_mute_music
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_default_mute_music)){
            //Clear the data name.
            line.erase(0,str_default_mute_music.length());

            Options::mute_music=Strings::string_to_bool(line);
        }

        //If the line ends the engine data.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</engine>")){
            return;
        }
    }
}

void Engine_Interface::load_window(File_IO_Load* load){
    windows.push_back(Window());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_name="name:";
        string str_starting_position="starting_position:";
        string str_starting_size="starting_size:";
        string str_width="width:";
        string str_height="height:";
        string str_fit_content="fit_content:";
        string str_title="title:";
        string str_font="font:";
        string str_font_color="font_color:";
        string str_border="border:";
        string str_background="background:";
        string str_scrolling_buttons="scrolling_buttons:";
        string str_close_function="close_function:";
        string str_disallow_closing="disallow_closing:";
        string str_exclusive="exclusive:";
        string str_no_button_sort="no_button_sort:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Name
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_name)){
            //Clear the data name.
            line.erase(0,str_name.length());

            windows[windows.size()-1].name=line;
        }
        //Starting position
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_starting_position)){
            //Clear the data name.
            line.erase(0,str_starting_position.length());

            vector<string> start_coords;
            boost::algorithm::split(start_coords,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].x=Strings::string_to_long(start_coords[0]);
            windows[windows.size()-1].y=Strings::string_to_long(start_coords[1]);

            windows[windows.size()-1].start_x=windows[windows.size()-1].x;
            windows[windows.size()-1].start_y=windows[windows.size()-1].y;
        }
        //Starting size
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_starting_size)){
            //Clear the data name.
            line.erase(0,str_starting_size.length());

            vector<string> start_dimensions;
            boost::algorithm::split(start_dimensions,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].start_width=Strings::string_to_long(start_dimensions[0]);
            windows[windows.size()-1].start_height=Strings::string_to_long(start_dimensions[1]);
        }
        //Width
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_width)){
            //Clear the data name.
            line.erase(0,str_width.length());

            windows[windows.size()-1].w=Strings::string_to_long(line);
        }
        //Height
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_height)){
            //Clear the data name.
            line.erase(0,str_height.length());

            windows[windows.size()-1].h=Strings::string_to_long(line);
        }
        //Fit Content
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fit_content)){
            //Clear the data name.
            line.erase(0,str_fit_content.length());

            windows[windows.size()-1].fit_content=Strings::string_to_bool(line);
        }
        //Title
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_title)){
            //Clear the data name.
            line.erase(0,str_title.length());

            windows[windows.size()-1].title=Strings::process_newlines(line);
        }
        //Font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font)){
            //Clear the data name.
            line.erase(0,str_font.length());

            windows[windows.size()-1].font=line;
        }
        //Font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_color)){
            //Clear the data name.
            line.erase(0,str_font_color.length());

            windows[windows.size()-1].font_color=line;
        }
        //Border
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_border)){
            //Clear the data name.
            line.erase(0,str_border.length());

            windows[windows.size()-1].border.name=line;
        }
        //Background
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_background)){
            //Clear the data name.
            line.erase(0,str_background.length());

            windows[windows.size()-1].background.name=line;
        }
        //Scrolling Buttons
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scrolling_buttons)){
            //Clear the data name.
            line.erase(0,str_scrolling_buttons.length());

            windows[windows.size()-1].scrolling_buttons=line;
        }
        //Close Function
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_close_function)){
            //Clear the data name.
            line.erase(0,str_close_function.length());

            windows[windows.size()-1].close_function=line;
        }
        //Disallow Closing
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_disallow_closing)){
            //Clear the data name.
            line.erase(0,str_disallow_closing.length());

            windows[windows.size()-1].disallow_closing=Strings::string_to_bool(line);
        }
        //Exclusive
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_exclusive)){
            //Clear the data name.
            line.erase(0,str_exclusive.length());

            windows[windows.size()-1].exclusive=Strings::string_to_bool(line);
        }
        //No Button Sort
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_no_button_sort)){
            //Clear the data name.
            line.erase(0,str_no_button_sort.length());

            windows[windows.size()-1].no_button_sort=Strings::string_to_bool(line);
        }

        //If the line begins an information.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"<info>")){
            load_information(load);
        }
        //If the line begins a button.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"<button>")){
            load_button(load);
        }

        //If the line ends the window.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</window>")){
            windows[windows.size()-1].set_dimensions();
            windows[windows.size()-1].center_in_game_window();

            return;
        }
    }
}

void Engine_Interface::load_information(File_IO_Load* load){
    windows[windows.size()-1].informations.push_back(Information());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_location="location:";
        string str_text="text:";
        string str_colored_text="colored_text:";
        string str_tooltip_text="tooltip:";
        string str_text_mutable="mutable:";
        string str_max_text_length="max_text_length:";
        string str_allowed_input="allowed_input:";
        string str_scrolling="scrolling:";
        string str_scroll_dimensions="box:";
        string str_sprite="sprite:";
        string str_background_type="background_type:";
        string str_background_opacity="background_opacity:";
        string str_font_color="font_color:";
        string str_special_info_text="special_text:";
        string str_special_info_sprite="special_sprite:";
        string str_font="font:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Location
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_location)){
            //Clear the data name.
            line.erase(0,str_location.length());

            vector<string> location;
            boost::algorithm::split(location,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].x=Strings::string_to_long(location[0]);
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].y=Strings::string_to_long(location[1]);

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].start_x=windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].x;
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].start_y=windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].y;
        }
        //Text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_text)){
            //Clear the data name.
            line.erase(0,str_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].text=Strings::process_newlines(line);
        }
        //Colored text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_colored_text)){
            //Clear the data name.
            line.erase(0,str_colored_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].set_colored_text(Strings::process_newlines(line));
        }
        //Tooltip text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_text)){
            //Clear the data name.
            line.erase(0,str_tooltip_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].tooltip_text=Strings::process_newlines(line);
        }
        //Text mutable
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_text_mutable)){
            //Clear the data name.
            line.erase(0,str_text_mutable.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].text_mutable=Strings::string_to_bool(line);
        }
        //Max text length
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_max_text_length)){
            //Clear the data name.
            line.erase(0,str_max_text_length.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].max_text_length=Strings::string_to_long(line);
        }
        //Allowed input
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_allowed_input)){
            //Clear the data name.
            line.erase(0,str_allowed_input.length());

            vector<string> inputs;
            boost::algorithm::split(inputs,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].allowed_input=inputs;
        }
        //Scrolling
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scrolling)){
            //Clear the data name.
            line.erase(0,str_scrolling.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].scrolling=Strings::string_to_bool(line);
        }
        //Scroll dimensions
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_scroll_dimensions)){
            //Clear the data name.
            line.erase(0,str_scroll_dimensions.length());

            vector<string> scroll_dimensions;
            boost::algorithm::split(scroll_dimensions,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].scroll_width=Strings::string_to_long(scroll_dimensions[0]);
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].scroll_height=Strings::string_to_long(scroll_dimensions[1]);
        }
        //Sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite)){
            //Clear the data name.
            line.erase(0,str_sprite.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].sprite.name=line;
        }
        //Background type
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_background_type)){
            //Clear the data name.
            line.erase(0,str_background_type.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].background_type=line;
        }
        //Background opacity
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_background_opacity)){
            //Clear the data name.
            line.erase(0,str_background_opacity.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].background_opacity=Strings::string_to_double(line);
        }
        //Font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_color)){
            //Clear the data name.
            line.erase(0,str_font_color.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].font_color=line;
        }
        //Special info text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_special_info_text)){
            //Clear the data name.
            line.erase(0,str_special_info_text.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].special_info_text=line;
        }
        //Special info sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_special_info_sprite)){
            //Clear the data name.
            line.erase(0,str_special_info_sprite.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].special_info_sprite=line;
        }
        //Font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font)){
            //Clear the data name.
            line.erase(0,str_font.length());

            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].font=line;
        }

        //If the line ends the information.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</info>")){
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].reset_cursor_position();
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].set_dimensions();
            windows[windows.size()-1].informations[windows[windows.size()-1].informations.size()-1].center_in_window(windows[windows.size()-1].w,windows[windows.size()-1].h);

            return;
        }
    }
}

void Engine_Interface::load_button(File_IO_Load* load){
    windows[windows.size()-1].buttons.push_back(Button());

    bool multi_line_comment=false;

    //As long as we haven't reached the end of the file.
    while(!load->eof()){
        string line="";

        //The option strings used in the file.

        string str_location="location:";
        string str_text="text:";
        string str_tooltip_text="tooltip:";
        string str_font="font:";
        string str_font_color="font_color:";
        string str_states="states:";
        string str_event_function="event:";
        string str_alt_function1="alt_event1:";
        string str_alt_function2="alt_event2:";
        string str_alt_function3="alt_event3:";
        string str_mouse_over_sound="sound_moused:";
        string str_event_fire_sound="sound_click:";
        string str_sprite="sprite:";
        string str_sprite_moused="sprite_moused:";
        string str_sprite_click="sprite_click:";

        //Grab the next line of the file.
        load->getline(&line);

        //Clear initial whitespace from the line.
        boost::algorithm::trim(line);

        //If the line ends a multi-line comment.
        if(boost::algorithm::contains(line,"*/")){
            multi_line_comment=false;
        }
        //If the line starts a multi-line comment.
        if(!multi_line_comment && boost::algorithm::starts_with(line,"/*")){
            multi_line_comment=true;
        }
        //If the line is a comment.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"//")){
            //Ignore this line.
        }

        //Load data based on the line.

        //Location
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_location)){
            //Clear the data name.
            line.erase(0,str_location.length());

            vector<string> location;
            boost::algorithm::split(location,line,boost::algorithm::is_any_of(","));

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].x=Strings::string_to_long(location[0]);
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].y=Strings::string_to_long(location[1]);

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].start_x=windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].x;
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].start_y=windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].y;
        }
        //Text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_text)){
            //Clear the data name.
            line.erase(0,str_text.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].text=Strings::process_newlines(line);
        }
        //Tooltip text
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_tooltip_text)){
            //Clear the data name.
            line.erase(0,str_tooltip_text.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].tooltip_text=Strings::process_newlines(line);
        }
        //Font
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font)){
            //Clear the data name.
            line.erase(0,str_font.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].font=line;
        }
        //Font color
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_color)){
            //Clear the data name.
            line.erase(0,str_font_color.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].font_color=line;
        }
        //States
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_states)){
            //Clear the data name.
            line.erase(0,str_states.length());

            vector<string> states;
            boost::algorithm::split(states,line,boost::algorithm::is_any_of(","));

            for(int i=0;i<states.size();i++){
                windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].states.push_back(states[i]);
            }
        }
        //Event function
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_event_function)){
            //Clear the data name.
            line.erase(0,str_event_function.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].event_function=line;
        }
        //Alt function 1
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_alt_function1)){
            //Clear the data name.
            line.erase(0,str_alt_function1.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].alt_function1=line;
        }
        //Alt function 2
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_alt_function2)){
            //Clear the data name.
            line.erase(0,str_alt_function2.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].alt_function2=line;
        }
        //Alt function 3
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_alt_function3)){
            //Clear the data name.
            line.erase(0,str_alt_function3.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].alt_function3=line;
        }
        //Mouse over sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mouse_over_sound)){
            //Clear the data name.
            line.erase(0,str_mouse_over_sound.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].mouse_over_sound=line;
        }
        //Event fire sound
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_event_fire_sound)){
            //Clear the data name.
            line.erase(0,str_event_fire_sound.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].event_fire_sound=line;
        }
        //Sprite
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite)){
            //Clear the data name.
            line.erase(0,str_sprite.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].sprite.name=line;
        }
        //Sprite moused
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite_moused)){
            //Clear the data name.
            line.erase(0,str_sprite_moused.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].sprite_moused.name=line;
        }
        //Sprite click
        else if(!multi_line_comment && boost::algorithm::starts_with(line,str_sprite_click)){
            //Clear the data name.
            line.erase(0,str_sprite_click.length());

            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].sprite_click.name=line;
        }

        //If the line ends the button.
        else if(!multi_line_comment && boost::algorithm::starts_with(line,"</button>")){
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].set_dimensions();
            windows[windows.size()-1].buttons[windows[windows.size()-1].buttons.size()-1].center_in_window(windows[windows.size()-1].w,windows[windows.size()-1].h);

            return;
        }
    }
}

Window* Engine_Interface::get_window(string name){
    Window* ptr_window=0;

    for(int i=0;i<windows.size();i++){
        if(windows[i].name==name){
            ptr_window=&windows[i];

            break;
        }
    }

    if(ptr_window==0){
        Log::add_error("Error accessing window '"+name+"'");
    }

    return ptr_window;
}

void Engine_Interface::rebuild_window_data(){
    for(int i=0;i<windows.size();i++){
        if(windows[i].need_to_rebuild_scrolling_buttons){
            windows[i].need_to_rebuild_scrolling_buttons=false;

            windows[i].build_scrolling_buttons();
        }
    }
}

bool Engine_Interface::is_window_open(Window* window){
    for(int i=0;i<window_z_order.size();i++){
        if(window_z_order[i]==window){
            return true;
        }
    }

    return false;
}

bool Engine_Interface::is_window_on_top(Window* window){
    if(window_z_order.size()>0 && window_z_order[0]==window){
        return true;
    }

    return false;
}

Window* Engine_Interface::get_top_window(){
    if(window_z_order.size()>0){
        return window_z_order[0];
    }

    return 0;
}

void Engine_Interface::open_window(Window* window){
    if(!is_window_open(window)){
        window_z_order.insert(window_z_order.begin(),window);

        reset_gui_selected_object();
    }
}

void Engine_Interface::close_window(Window* window){
    for(int i=0;i<window_z_order.size();i++){
        if(window_z_order[i]==window){
            window_z_order.erase(window_z_order.begin()+i);

            reset_gui_selected_object();

            break;
        }
    }
}

void Engine_Interface::bring_window_to_top(Window* window){
    if(is_window_open(window)){
        close_window(window);

        open_window(window);
    }
}

bool Engine_Interface::is_exclusive_window_open(){
    for(int i=0;i<windows.size();i++){
        if(windows[i].on && windows[i].exclusive){
            return true;
        }
    }

    return false;
}

bool Engine_Interface::is_any_window_open(){
    for(int i=0;i<windows.size();i++){
        if(windows[i].on){
            return true;
        }
    }

    return false;
}

int Engine_Interface::open_window_count(){
    int open_windows=0;

    for(int i=0;i<windows.size();i++){
        if(windows[i].on){
            open_windows++;
        }
    }

    return open_windows;
}

void Engine_Interface::close_all_windows(Window* ignore){
    for(int i=0;i<windows.size();i++){
        if(&windows[i]!=ignore){
            windows[i].toggle_on(true,false);
        }
    }
}

void Engine_Interface::close_top_window(){
    if(is_any_window_open()){
        Window* window=window_z_order[0];

        if(!window->disallow_closing){
            window->toggle_on(true,false);
        }
    }
}

bool Engine_Interface::is_mouse_over_window(){
    if(window_under_mouse!=0){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::is_mouse_over_open_window(){
    if(window_under_mouse!=0 && is_window_open(window_under_mouse)){
        return true;
    }
    else{
        return false;
    }
}

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

bool Engine_Interface::mouse_allowed(){
    if(!Controller_Manager::touch_controls){
        return true;
    }
    else{
        return false;
    }
}

void Engine_Interface::gui_check_scrolling_button(){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                if(top_window->is_button_scrolling(object.index)){
                    for(int i=0;i<10000 && !top_window->is_scrolling_button_on_screen(object.index);i++){
                        if(top_window->off_screen_scrolling_button_direction(object.index)){
                            top_window->scroll_up();
                        }
                        else{
                            top_window->scroll_down();
                        }
                    }
                }
            }
        }
    }
}

void Engine_Interface::reset_gui_selector_chasers(){
    for(int i=0;i<gui_selector_chasers.size();i++){
        gui_selector_chasers[i].x=-1.0;
        gui_selector_chasers[i].y=-1.0;
    }
}

void Engine_Interface::reset_gui_selected_object(){
    gui_selected_object=-1;

    if(gui_mode=="keyboard" || gui_mode=="controller"){
        if(get_gui_object_count()>0){
            gui_selected_object=0;
        }
    }

    gui_check_scrolling_button();

    reset_gui_selector_chasers();
}

void Engine_Interface::set_gui_mode(string new_gui_mode){
    string previous_gui_mode=gui_mode;

    gui_mode=new_gui_mode;

    if(previous_gui_mode!=gui_mode){
        //Only reset selected object if we are switching to or from mouse mode.
        if(previous_gui_mode=="mouse" || gui_mode=="mouse"){
            bool was_console=is_console_selected();
            bool was_chat=is_chat_selected();

            clear_mutable_info();

            reset_gui_selected_object();

            if(was_console){
                console.toggle_on();
            }
            else if(was_chat){
                chat.toggle_on();
            }
        }

        if(gui_mode!="mouse" && gui_mode!="keyboard" && gui_mode!="controller"){
            Log::add_error("Error setting GUI mode '"+gui_mode+"'");
        }
    }
}

int Engine_Interface::get_gui_object_count(){
    Window* top_window=get_top_window();

    if(top_window!=0){
        int button_count=0;
        int info_count=0;

        for(int i=0;i<top_window->buttons.size();i++){
            if(!top_window->buttons[i].is_x && !top_window->buttons[i].is_divider){
                button_count++;
            }
        }

        for(int i=0;i<top_window->informations.size();i++){
            if(top_window->informations[i].text_mutable || top_window->informations[i].scrolling){
                info_count++;
            }
        }

        return button_count+info_count;
    }
    else{
        return 0;
    }
}

void Engine_Interface::change_gui_selected_object(string direction){
    int object_count=get_gui_object_count();
    int fast_nav=(int)ceil((double)object_count*0.1);

    if(object_count>0){
        clear_mutable_info();

        if(direction=="up"){
            if(--gui_selected_object<0){
                gui_selected_object=object_count-1;
            }
        }
        else if(direction=="down"){
            if(++gui_selected_object>object_count-1){
                gui_selected_object=0;
            }
        }
        else if(direction=="left"){
            gui_selected_object-=fast_nav;

            if(gui_selected_object<0){
                gui_selected_object=object_count-1;
            }
        }
        else if(direction=="right"){
            gui_selected_object+=fast_nav;

            if(gui_selected_object>object_count-1){
                gui_selected_object=0;
            }
        }
        else{
            Log::add_error("Error navigating GUI in direction '"+direction+"'");
        }

        gui_check_scrolling_button();

        reset_gui_selector_chasers();
    }
    else{
        reset_gui_selected_object();
    }
}

GUI_Object Engine_Interface::get_gui_selected_object(){
    GUI_Object object("none",-1,-1,-1);

    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            vector<GUI_Object> objects;

            for(int i=0;i<top_window->buttons.size();i++){
                if(!top_window->buttons[i].is_x && !top_window->buttons[i].is_divider){
                    int real_y=top_window->buttons[i].y;

                    //If this button is non-scrolling and above the scrolling button area, add it normally.
                    if(!top_window->is_button_scrolling(i) && top_window->buttons[i].y<Engine_Data::scrolling_buttons_offset){
                    }
                    //If this button is scrolling, place it at the top of the scrolling button area, offset by its position in the scrolling buttons list.
                    else if(top_window->is_button_scrolling(i)){
                        real_y=Engine_Data::scrolling_buttons_offset+top_window->get_scrolling_button_position(i);
                    }
                    //If this button is non-scrolling and below the scrolling button area, offset it by the bottommost scrolling button's height.
                    else{
                        if(top_window->get_scrolling_button_count()>0){
                            real_y+=Engine_Data::scrolling_buttons_offset+top_window->get_scrolling_button_count()-1;
                        }
                    }

                    objects.push_back(GUI_Object("button",i,top_window->buttons[i].x,real_y));
                }
            }

            for(int i=0;i<top_window->informations.size();i++){
                if(top_window->informations[i].text_mutable || top_window->informations[i].scrolling){
                    int real_y=top_window->informations[i].y;

                    //If there are scrolling buttons and this info is below the scrolling button area, offset it by the bottommost scrolling button's height.
                    if(top_window->get_scrolling_button_count()>0 && top_window->informations[i].y>=Engine_Data::scrolling_buttons_offset){
                        real_y+=Engine_Data::scrolling_buttons_offset+top_window->get_scrolling_button_count()-1;
                    }

                    objects.push_back(GUI_Object("information",i,top_window->informations[i].x,real_y));
                }
            }

            if(objects.size()>gui_selected_object){
                if(!top_window->no_button_sort){
                    GUI_Object::set_sort_by_y(objects,true);
                    Sorting::quick_sort(objects);

                    //Build a list of all y values that have at least one duplicate
                    vector<int> y_duplicates_to_sort;
                    vector<int> y_values;
                    for(int i=0;i<objects.size();i++){
                        if(y_values.size()==0){
                            y_values.push_back(objects[i].y);
                        }
                        else{
                            for(int j=0;j<y_values.size();j++){
                                if(objects[i].y==y_values[j]){
                                    if(y_duplicates_to_sort.size()==0){
                                        y_duplicates_to_sort.push_back(y_values[j]);
                                    }
                                    else{
                                        for(int k=0;k<y_duplicates_to_sort.size();k++){
                                            if(y_values[j]==y_duplicates_to_sort[k]){
                                                break;
                                            }

                                            if(k==y_duplicates_to_sort.size()-1){
                                                y_duplicates_to_sort.push_back(y_values[j]);

                                                break;
                                            }
                                        }
                                    }

                                    break;
                                }

                                if(j==y_values.size()-1){
                                    y_values.push_back(objects[i].y);

                                    break;
                                }
                            }
                        }
                    }

                    for(int i=0;i<y_duplicates_to_sort.size();i++){
                        vector<GUI_Object> replacements;
                        int initial_object=-1;

                        for(int j=0;j<objects.size();j++){
                            if(objects[j].y==y_duplicates_to_sort[i]){
                                if(initial_object==-1){
                                    initial_object=j;
                                }

                                replacements.push_back(objects[j]);
                            }
                        }

                        GUI_Object::set_sort_by_y(replacements,false);
                        Sorting::quick_sort(replacements);

                        for(int j=0;j<replacements.size();j++){
                            objects[initial_object+j]=replacements[j];
                        }
                    }
                }

                object=objects[gui_selected_object];
            }
        }

        if(object.type=="none"){
            reset_gui_selected_object();
        }
    }

    return object;
}

Engine_Rect Engine_Interface::get_gui_selected_object_pos(){
    Engine_Rect object_pos;

    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                Button* button=&top_window->buttons[object.index];

                object_pos.x=top_window->x+button->x;
                object_pos.y=top_window->y+button->y;
                object_pos.w=button->w;
                object_pos.h=button->h;

                if(top_window->is_button_scrolling(object.index)){
                    object_pos.y=top_window->get_scrolling_button_offset(object.index)+button->y;
                }
            }
            else if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                object_pos.x=top_window->x+info->x;
                object_pos.y=top_window->y+info->y;
                object_pos.w=info->w;
                object_pos.h=info->h;
            }
        }
    }

    return object_pos;
}

bool Engine_Interface::is_gui_object_selected(Button* button_to_check){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                Button* button=&top_window->buttons[object.index];

                if(button_to_check==button){
                    return true;
                }
            }
        }
    }

    return false;
}

bool Engine_Interface::is_gui_object_selected(Information* info_to_check){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                if(info_to_check==info){
                    return true;
                }
            }
        }
    }

    return false;
}

void Engine_Interface::confirm_gui_object(){
    if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="button"){
                Button* button=&top_window->buttons[object.index];

                button->mouse_button_down();
                button->mouse_button_up(top_window);
                button->reset_clicked();
            }
            else if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                if(info->text_mutable){
                    info->begin_editing();
                }
            }
        }
    }
}

void Engine_Interface::scroll_gui_object_up(){
    if(is_console_selected()){
        console.info_display.scroll_up(0);
    }
    else if(is_chat_selected()){
        chat.info_display.scroll_up(0);
    }
    else if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                info->scroll_up(top_window->y);
            }
        }
    }
}

void Engine_Interface::scroll_gui_object_down(){
    if(is_console_selected()){
        console.info_display.scroll_down(0);
    }
    else if(is_chat_selected()){
        chat.info_display.scroll_down(0);
    }
    else if(gui_selected_object!=-1){
        Window* top_window=get_top_window();

        if(top_window!=0){
            GUI_Object object=get_gui_selected_object();

            if(object.type=="information"){
                Information* info=&top_window->informations[object.index];

                info->scroll_down(top_window->y);
            }
        }
    }
}

void Engine_Interface::get_mouse_state(int* mouse_x,int* mouse_y){
    SDL_Rect rect;
    Game_Window::get_renderer_viewport(&rect);

    float scale_x=0.0f;
    float scale_y=0.0f;
    Game_Window::get_renderer_scale(&scale_x,&scale_y);

    SDL_GetMouseState(mouse_x,mouse_y);

    float offset_x=(float)rect.x*scale_x;
    float offset_y=(float)rect.y*scale_y;

    *mouse_x=(int)ceil(((float)*mouse_x-offset_x)/scale_x);
    *mouse_y=(int)ceil(((float)*mouse_y-offset_y)/scale_y);
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

void Engine_Interface::set_mutable_info(Information* ptr_info){
    ptr_mutable_info=ptr_info;

    if(mutable_info_selected()){
        ptr_mutable_info->check_cursor_position();

        if(allow_screen_keyboard()){
            SDL_StartTextInput();
        }
        else if(Engine_Data::controller_text_entry_small && gui_mode=="controller"){
            text_entry_small_selector.x=0;
            text_entry_small_selector.y=0;
        }
    }
}

void Engine_Interface::clear_mutable_info(){
    ptr_mutable_info=0;

    if(Game_Window::is_screen_keyboard_shown()){
        SDL_StopTextInput();
    }

    if(console.on){
        console.toggle_on();
    }
    if(chat.on){
        chat.toggle_on();
    }
}

bool Engine_Interface::mutable_info_selected(){
    if(ptr_mutable_info!=0){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::mutable_info_this(Information* ptr_info){
    if(ptr_mutable_info==ptr_info){
        return true;
    }
    else{
        return false;
    }
}

void Engine_Interface::add_chat(string message){
    chat.add_text(message);
}

void Engine_Interface::make_notice(string message){
    Window* window=get_window("notice");

    window->informations[0].set_text(message);

    window->set_dimensions();

    window->toggle_on(true,true);
}

void Engine_Interface::make_toast(string message,string length,int custom_length){
    if(message.length()>0){
        int real_length=0;

        if(custom_length>-1){
            real_length=custom_length;
        }
        else{
            if(length=="short"){
                real_length=Engine_Data::toast_length_short;
            }
            else if(length=="medium"){
                real_length=Engine_Data::toast_length_medium;
            }
            else if(length=="long"){
                real_length=Engine_Data::toast_length_long;
            }
            else{
                real_length=Engine_Data::toast_length_medium;
            }
        }

        //Real length was in terms of seconds. Now we translate it to frames.
        real_length=(int)ceil((double)real_length*Engine::UPDATE_RATE);
        if(real_length<1){
            real_length=1;
        }

        toasts.push_back(Toast(message,1.0/(double)real_length));
    }
}

void Engine_Interface::handle_text_input(string text){
    if(mutable_info_selected()){
        const uint8_t* keystates=SDL_GetKeyboardState(NULL);

        //Remove characters that are not allowed.
        for(int i=0;i<text.length();i++){
            bool keep_char=false;

            if((ptr_mutable_info->allows_input("letters") && ((text[i]>='A' && text[i]<='Z') || (text[i]>='a' && text[i]<='z'))) ||
               (ptr_mutable_info->allows_input("numbers") && text[i]>='0' && text[i]<='9') ||
               (ptr_mutable_info->allows_input("symbols") && ((text[i]>='!' && text[i]<='/') || (text[i]>=':' && text[i]<='@') || (text[i]>='[' && text[i]<='`') || ((unsigned char)text[i]>='{'))) ||
               (ptr_mutable_info->allows_input("symbols_posix") && (text[i]=='.' || text[i]=='_' || text[i]=='-')) ||
               (ptr_mutable_info->allows_input("space") && (text[i]==' ' || text[i]=='\t'))){
                //Only allow the console toggle key's grave character to be typed if we are not in the console
                if(!is_console_selected() ||
                   !(text[i]=='`' && (gui_mode=="mouse" || gui_mode=="keyboard") && !keystates[SDL_SCANCODE_LSHIFT] && !keystates[SDL_SCANCODE_RSHIFT])){
                    keep_char=true;
                }
            }

            if(!keep_char){
                text.erase(text.begin()+i);
                i--;
            }
        }

        //Convert tabs into spaces
        //I realize this is not ideal (or even correct),
        //but I think it's good enough, and implementing real tabs would be a headache
        string tab="";
        for(int i=0;i<Engine_Data::spaces_per_tab;i++){
            tab+=" ";
        }
        boost::algorithm::replace_all(text,"\t",tab);

        //Don't allow the mutable string to exceed its maximum size.
        for(int i=text.length()-1;i>=0 && ptr_mutable_info->text.length()+text.length()>ptr_mutable_info->max_text_length;i--){
            text.erase(text.begin()+i);
        }

        if(text.length()>0){
            ptr_mutable_info->add_text(text);
        }
    }
}

int Engine_Interface::game_command_state(string name){
    for(int i=0;i<game_commands.size();i++){
        if(game_commands[i].name==name){
            const uint8_t* keystates=SDL_GetKeyboardState(NULL);

            if(game_commands[i].key_valid() && keystates[game_commands[i].key]){
                return true;
            }
            else if(game_commands[i].button_valid() && Controller_Manager::controller_state(-1,game_commands[i].button)){
                return true;
            }
            else if(game_commands[i].axis_valid()){
                int state=Controller_Manager::controller_state(-1,game_commands[i].axis);

                if(state!=0){
                    return state;
                }
            }
        }
    }

    return false;
}

void Engine_Interface::gui_nav_back_android(){
    string window_name="main_menu";
    Window* window=get_window(window_name);

    if(mutable_info_selected()){
        clear_mutable_info();
    }
    else if(is_any_window_open()){
        if(!Game_Manager::in_progress && is_window_open(window) && open_window_count()==1){
            quit();
        }
        else{
            close_top_window();
        }
    }
    else{
        quit();
    }
}

void Engine_Interface::gui_nav_toggle_menu_android(){
    string window_name="main_menu";
    if(Game_Manager::in_progress){
        window_name="ingame_menu";
    }

    Window* window=get_window(window_name);

    if(is_window_open(window) && !is_window_on_top(window)){
        bring_window_to_top(window);
    }
    else{
        window->toggle_on();
    }
}

void Engine_Interface::gui_nav_back_controller(){
    set_gui_mode("controller");

    if(mutable_info_selected()){
        clear_mutable_info();
    }
    else if(is_any_window_open()){
        close_top_window();
    }
}

void Engine_Interface::gui_nav_toggle_menu_controller(){
    set_gui_mode("controller");

    string window_name="main_menu";
    if(Game_Manager::in_progress){
        window_name="ingame_menu";
    }

    Window* window=get_window(window_name);

    if(is_window_open(window) && !is_window_on_top(window)){
        bring_window_to_top(window);
    }
    else{
        window->toggle_on();
    }
}

void Engine_Interface::gui_nav_back_keyboard_and_mouse(){
    if(gui_mode=="controller"){
        set_gui_mode("mouse");
    }

    if(mutable_info_selected()){
        clear_mutable_info();
    }
    else if(is_any_window_open()){
        close_top_window();
    }
    else{
        if(Game_Manager::in_progress){
            get_window("ingame_menu")->toggle_on();
        }
        else{
            get_window("main_menu")->toggle_on();
        }
    }
}

void Engine_Interface::gui_nav_confirm(string gui_type){
    set_gui_mode(gui_type);

    confirm_gui_object();
}

void Engine_Interface::gui_nav_up(string gui_type){
    set_gui_mode(gui_type);

    change_gui_selected_object("up");
}

void Engine_Interface::gui_nav_down(string gui_type){
    set_gui_mode(gui_type);

    change_gui_selected_object("down");
}

void Engine_Interface::gui_nav_left(){
    if(mutable_info_selected()){
        ptr_mutable_info->move_cursor_left();
    }
}

void Engine_Interface::gui_nav_right(){
    if(mutable_info_selected()){
        ptr_mutable_info->move_cursor_right();
    }
}

void Engine_Interface::gui_scroll_up(string gui_type){
    set_gui_mode(gui_type);

    scroll_gui_object_up();
}

void Engine_Interface::gui_scroll_down(string gui_type){
    set_gui_mode(gui_type);

    scroll_gui_object_down();
}

bool Engine_Interface::is_console_selected(){
    if(mutable_info_selected() && mutable_info_this(&console.info_input)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::is_chat_selected(){
    if(mutable_info_selected() && mutable_info_this(&chat.info_input)){
        return true;
    }
    else{
        return false;
    }
}

int Engine_Interface::get_text_input_selected_chunk(){
    int selected_chunk=-1;

    int axis_x=Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_LEFTX);
    if(axis_x>-Engine_Data::controller_dead_zone && axis_x<Engine_Data::controller_dead_zone){
        axis_x=0;
    }
    int axis_y=Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_LEFTY);
    if(axis_y>-Engine_Data::controller_dead_zone && axis_y<Engine_Data::controller_dead_zone){
        axis_y=0;
    }

    if(axis_x!=0 || axis_y!=0){
        Collision_Rect box_a(0,0,1,1);
        Collision_Rect box_b(axis_x,axis_y,1,1);

        double axis_angle=Collision::get_angle_to_rect(box_a,box_b,Collision_Rect(0.0,0.0,0.0,0.0));

        if(axis_angle>=74 && axis_angle<118){
            selected_chunk=0;
        }
        else if(axis_angle>=30 && axis_angle<74){
            selected_chunk=1;
        }
        else if(axis_angle>=338 || axis_angle<30){
            selected_chunk=2;
        }
        else if(axis_angle>=294 && axis_angle<338){
            selected_chunk=3;
        }
        else if(axis_angle>=250 && axis_angle<294){
            selected_chunk=4;
        }
        else if(axis_angle>=206 && axis_angle<250){
            selected_chunk=5;
        }
        else if(axis_angle>=162 && axis_angle<206){
            selected_chunk=6;
        }
        else if(axis_angle>=118 && axis_angle<162){
            selected_chunk=7;
        }
    }

    return selected_chunk;
}

vector<string>* Engine_Interface::get_text_input_character_set(){
    vector<string>* characters=&characters_lower;
    if(Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_TRIGGERLEFT)>Engine_Data::controller_dead_zone){
        characters=&characters_upper;
    }
    else if(Controller_Manager::controller_state(-1,SDL_CONTROLLER_AXIS_TRIGGERRIGHT)>Engine_Data::controller_dead_zone){
        characters=&characters_numbers;
    }

    return characters;
}

void Engine_Interface::input_backspace(){
    if(mutable_info_selected()){
        ptr_mutable_info->input_backspace();
    }
}

void Engine_Interface::input_delete(){
    if(mutable_info_selected()){
        ptr_mutable_info->input_delete();
    }
}

void Engine_Interface::input_newline(){
    if(mutable_info_selected()){
        ptr_mutable_info->input_newline();
    }
}

bool Engine_Interface::allow_screen_keyboard(){
    if(Options::screen_keyboard && SDL_HasScreenKeyboardSupport()){
        return true;
    }
    else{
        return false;
    }
}

string Engine_Interface::get_text_entry_small_character(){
    if(text_entry_small_selector.x<0){
        text_entry_small_selector.x=0;
    }
    else if(text_entry_small_selector.x>12){
        text_entry_small_selector.x=12;
    }

    if(text_entry_small_selector.y<0){
        text_entry_small_selector.y=0;
    }
    else if(text_entry_small_selector.y>7){
        text_entry_small_selector.y=7;
    }

    string choices[13][8];

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][0]=characters_upper[i];
    }
    for(int i=13,n=0;i<26;i++,n++){
        choices[n][1]=characters_upper[i];
    }

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][2]=characters_lower[i];
    }
    for(int i=13,n=0;i<26;i++,n++){
        choices[n][3]=characters_lower[i];
    }

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][4]=characters_numbers[i];
    }

    for(int i=0,n=0;i<13;i++,n++){
        choices[n][5]=characters_symbols[i];
    }
    for(int i=13,n=0;i<26;i++,n++){
        choices[n][6]=characters_symbols[i];
    }
    for(int i=26,n=0;i<39;i++,n++){
        choices[n][7]=characters_symbols[i];
    }

    return choices[text_entry_small_selector.x][text_entry_small_selector.y];
}

void Engine_Interface::prepare_for_input(){
    //Start each frame of updates with the tooltip off.
    tooltip.on=false;

    mouse_over=false;

    if(counter_gui_scroll_axis>0){
        counter_gui_scroll_axis--;
    }

    window_under_mouse=0;

    if(allow_screen_keyboard() && !Game_Window::is_screen_keyboard_shown() && mutable_info_selected()){
        clear_mutable_info();
    }

    vector<int> closed_windows;
    for(int i=0;i<windows.size();i++){
        if(!is_window_open(&windows[i])){
            closed_windows.push_back(i);
        }
    }

    int mouse_x=0;
    int mouse_y=0;
    get_mouse_state(&mouse_x,&mouse_y);

    for(int i=0;i<window_z_order.size() && window_under_mouse==0;i++){
        Collision_Rect box_a(mouse_x,mouse_y,Engine_Data::cursor_width,Engine_Data::cursor_height);
        Collision_Rect box_b(window_z_order[i]->x,window_z_order[i]->y,window_z_order[i]->w,window_z_order[i]->h);

        if(Collision::check_rect(box_a,box_b)){
            window_under_mouse=window_z_order[i];
        }
    }

    for(int i=0;i<closed_windows.size() && window_under_mouse==0;i++){
        Collision_Rect box_a(mouse_x,mouse_y,Engine_Data::cursor_width,Engine_Data::cursor_height);
        Collision_Rect box_b(windows[closed_windows[i]].x,windows[closed_windows[i]].y,windows[closed_windows[i]].w,windows[closed_windows[i]].h);

        if(Collision::check_rect(box_a,box_b)){
            window_under_mouse=&windows[closed_windows[i]];
        }
    }
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

bool Engine_Interface::handle_input_events_command_set(){
    bool event_consumed=false;

    if(configure_command!=-1){
        const uint8_t* keystates=SDL_GetKeyboardState(NULL);

        bool allow_keys_and_buttons=true;
        bool allow_axes=true;

        if(configure_command<game_commands.size()){
            const char* ckey=SDL_GetScancodeName(game_commands[configure_command].key);
            const char* cbutton=SDL_GameControllerGetStringForButton(game_commands[configure_command].button);
            const char* caxis=SDL_GameControllerGetStringForAxis(game_commands[configure_command].axis);

            if(caxis!=0 && game_commands[configure_command].axis!=SDL_CONTROLLER_AXIS_INVALID){
                allow_keys_and_buttons=false;
            }
            else{
                allow_axes=false;
            }
        }

        switch(Engine::event.type){
            case SDL_CONTROLLERBUTTONDOWN:
                if(!event_consumed && (Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_START || allow_keys_and_buttons)){
                    if(Engine::event.cbutton.button!=SDL_CONTROLLER_BUTTON_START){
                        if(configure_command<game_commands.size()){
                            game_commands[configure_command].button=(SDL_GameControllerButton)Engine::event.cbutton.button;
                        }

                        save_game_commands();
                    }

                    get_window("configure_command")->toggle_on(true,false);

                    event_consumed=true;
                }
                break;

            case SDL_KEYDOWN:
                if(!event_consumed && Engine::event.key.repeat==0 &&
                   (Engine::event.key.keysym.scancode==SDL_SCANCODE_ESCAPE || Engine::event.key.keysym.scancode==SDL_SCANCODE_AC_BACK || allow_keys_and_buttons)){
                    if(Engine::event.key.keysym.scancode!=SDL_SCANCODE_ESCAPE && Engine::event.key.keysym.scancode!=SDL_SCANCODE_AC_BACK && Engine::event.key.keysym.scancode!=SDL_SCANCODE_MENU){
                        if(configure_command<game_commands.size()){
                            game_commands[configure_command].key=Engine::event.key.keysym.scancode;
                        }

                        save_game_commands();
                    }

                    get_window("configure_command")->toggle_on(true,false);

                    event_consumed=true;
                }
                break;

            case SDL_CONTROLLERAXISMOTION:
                if(!event_consumed && allow_axes){
                    if(configure_command<game_commands.size()){
                        game_commands[configure_command].axis=(SDL_GameControllerAxis)Engine::event.caxis.axis;
                    }

                    save_game_commands();
                    get_window("configure_command")->toggle_on(true,false);

                    event_consumed=true;
                }
                break;
        }
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
            if(mutable_info_selected()){
                if(!event_consumed){
                    string text=Engine::event.text.text;

                    handle_text_input(text);

                    event_consumed=true;
                }
            }
            break;

        case SDL_CONTROLLERAXISMOTION:
            if(!event_ignore_command_set){
                //GUI nav controller axis up
                if(!event_consumed && is_any_window_open() && !console.on && !mutable_info_selected() && gui_axis_nav_last_direction!="up" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTY && Engine::event.caxis.value<-Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="up";

                    gui_nav_up("controller");

                    event_consumed=true;
                }
                //GUI nav controller axis down
                if(!event_consumed && is_any_window_open() && !console.on && !mutable_info_selected() && gui_axis_nav_last_direction!="down" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTY && Engine::event.caxis.value>Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="down";

                    gui_nav_down("controller");

                    event_consumed=true;
                }
                //GUI nav controller axis left
                if(!event_consumed && mutable_info_selected() && gui_axis_nav_last_direction!="left" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTX && Engine::event.caxis.value<-Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="left";

                    gui_nav_left();

                    event_consumed=true;
                }
                //GUI nav controller axis right
                if(!event_consumed && mutable_info_selected() && gui_axis_nav_last_direction!="right" &&
                   Engine::event.caxis.axis==SDL_CONTROLLER_AXIS_LEFTX && Engine::event.caxis.value>Engine_Data::controller_dead_zone){
                    gui_axis_nav_last_direction="right";

                    gui_nav_right();

                    event_consumed=true;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            if(!event_ignore_command_set){
                if(mutable_info_selected() && !allow_screen_keyboard()){
                    if(!Engine_Data::controller_text_entry_small){
                        if(!event_consumed && ptr_mutable_info->allows_input("newline") && ptr_mutable_info->text.length()<ptr_mutable_info->max_text_length &&
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

                        if(!event_consumed && ptr_mutable_info->allows_input("backspace") && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_LEFTSHOULDER && ptr_mutable_info->text.length()>0){
                            input_backspace();

                            event_consumed=true;
                        }
                        if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_RIGHTSHOULDER){
                            handle_text_input(" ");

                            event_consumed=true;
                        }
                        if(!event_consumed && (Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_A || Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_B ||
                                               Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_X || Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_Y)){
                            vector<string>* characters=get_text_input_character_set();
                            int selected_chunk=get_text_input_selected_chunk();

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

                                handle_text_input(characters->at((selected_chunk*4)+character_offset));

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
                            if(ptr_mutable_info->allows_input("backspace") && ptr_mutable_info->text.length()>0){
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
                            string text_entry_char=get_text_entry_small_character();

                            if((int)text_entry_char[0]==1){
                                if(ptr_mutable_info->allows_input("backspace") && ptr_mutable_info->text.length()>0){
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
                                    clear_mutable_info();
                                }
                            }
                            else{
                                handle_text_input(text_entry_char);
                            }

                            Sound_Manager::play_sound("text_input_small_select");

                            event_consumed=true;
                        }
                    }
                }

                //GUI nav controller back
                if(!event_consumed && (mutable_info_selected() || is_any_window_open()) && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_B){
                    gui_nav_back_controller();

                    event_consumed=true;
                }
                //GUI nav controller toggle menu
                if(!event_consumed && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_START){
                    gui_nav_toggle_menu_controller();

                    event_consumed=true;
                }
                //GUI nav controller confirm
                if(!event_consumed && !mutable_info_selected() && is_any_window_open() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                    gui_nav_confirm("controller");

                    event_consumed=true;
                }
                //GUI nav controller up
                if(!event_consumed && is_any_window_open() && !console.on && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){
                    gui_nav_up("controller");

                    event_consumed=true;
                }
                //GUI nav controller down
                if(!event_consumed && is_any_window_open() && !console.on && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){
                    gui_nav_down("controller");

                    event_consumed=true;
                }
                //GUI nav controller left
                if(!event_consumed && mutable_info_selected() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_LEFT){
                    gui_nav_left();

                    event_consumed=true;
                }
                //GUI nav controller right
                if(!event_consumed && mutable_info_selected() && Engine::event.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT){
                    gui_nav_right();

                    event_consumed=true;
                }
            }
            break;

        case SDL_KEYDOWN:
            if(!event_ignore_command_set){
                if(mutable_info_selected()){
                    if(!event_consumed && ptr_mutable_info->allows_input("newline") && ptr_mutable_info->text.length()<ptr_mutable_info->max_text_length &&
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

                    if(!event_consumed && ptr_mutable_info->allows_input("backspace") && Engine::event.key.keysym.scancode==SDL_SCANCODE_BACKSPACE && ptr_mutable_info->text.length()>0){
                        input_backspace();

                        event_consumed=true;
                    }

                    if(!event_consumed && ptr_mutable_info->allows_input("backspace") && Engine::event.key.keysym.scancode==SDL_SCANCODE_DELETE && ptr_mutable_info->text.length()>0){
                        input_delete();

                        event_consumed=true;
                    }

                    if(!event_consumed && !is_console_selected() && ptr_mutable_info->allows_input("space") && Engine::event.key.keysym.scancode==SDL_SCANCODE_TAB){
                        handle_text_input("\t");

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
                        change_option("cl_fullscreen_state",Strings::bool_to_string(!Options::fullscreen));

                        Engine_Mailman::send_letter("reload");

                        event_consumed=true;
                    }

                    //Quit game
                    if(!event_consumed && (keystates[SDL_SCANCODE_LALT] || keystates[SDL_SCANCODE_RALT]) && Engine::event.key.keysym.scancode==SDL_SCANCODE_F4){
                        quit();

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
                    if(!event_consumed && !mutable_info_selected() && is_any_window_open() && (Engine::event.key.keysym.scancode==SDL_SCANCODE_RETURN || Engine::event.key.keysym.scancode==SDL_SCANCODE_KP_ENTER)){
                        gui_nav_confirm("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard up
                    if(!event_consumed && is_any_window_open() && !console.on && Engine::event.key.keysym.scancode==SDL_SCANCODE_UP){
                        gui_nav_up("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard down
                    if(!event_consumed && is_any_window_open() && !console.on && Engine::event.key.keysym.scancode==SDL_SCANCODE_DOWN){
                        gui_nav_down("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard left
                    if(!event_consumed && mutable_info_selected() && Engine::event.key.keysym.scancode==SDL_SCANCODE_LEFT){
                        gui_nav_left();

                        event_consumed=true;
                    }
                    //GUI nav keyboard right
                    if(!event_consumed && mutable_info_selected() && Engine::event.key.keysym.scancode==SDL_SCANCODE_RIGHT){
                        gui_nav_right();

                        event_consumed=true;
                    }
                    //GUI nav keyboard scroll up
                    if(!event_consumed && (is_any_window_open() || console.on || chat.on) && Engine::event.key.keysym.scancode==SDL_SCANCODE_PAGEUP){
                        gui_scroll_up("keyboard");

                        event_consumed=true;
                    }
                    //GUI nav keyboard scroll down
                    if(!event_consumed && (is_any_window_open() || console.on || chat.on) && Engine::event.key.keysym.scancode==SDL_SCANCODE_PAGEDOWN){
                        gui_scroll_down("keyboard");

                        event_consumed=true;
                    }
                }

                //Paste
                if(!event_consumed && (keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL]) && Engine::event.key.keysym.scancode==SDL_SCANCODE_V){
					if(SDL_HasClipboardText() && mutable_info_selected()){
                        char* text=SDL_GetClipboardText();
                        string str_text=text;
                        SDL_free(text);

                        vector<string> lines;
                        boost::algorithm::split(lines,str_text,boost::algorithm::is_any_of("\n"));

                        for(int i=0;i<lines.size();i++){
                            handle_text_input(lines[i]);

                            if(i<lines.size()-1 && ptr_mutable_info->allows_input("newline") && ptr_mutable_info->text.length()<ptr_mutable_info->max_text_length){
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
            if(!event_consumed && mouse_allowed()){
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

    if(!console.on && !event_ignore_command_set && mouse_allowed()){
        if(window_under_mouse!=0){
            if(!event_consumed){
                event_consumed=window_under_mouse->handle_input_events();
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
        Window* top_window=get_top_window();

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

    if(is_any_window_open() || console.on || chat.on){
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
    get_mouse_state(&mouse_x,&mouse_y);

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
    msg+="Current Gui Mode: "+Strings::first_letter_capital(gui_mode)+"\n";

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
    for(int i=0;i<windows.size();i++){
        windows[i].animate();
    }

    console.animate();
    chat.animate();

    if(toasts.size()>0){
        toasts[0].animate();

        if(toasts[0].is_done()){
            toasts.erase(toasts.begin());
        }
    }

    Object_Manager::animate_cursors();

    animate_gui_selector_chasers();

    text_selector.animate();

    //Update the text cursor.
    if(++counter_cursor>=(int)ceil((48.0/1000.0)*Engine::UPDATE_RATE)){
        counter_cursor=0;

        if(cursor_opacity==10){
            cursor_fade_direction=false;
        }
        else if(cursor_opacity==0){
            cursor_fade_direction=true;
        }

        if(cursor_fade_direction && cursor_opacity<10){
            cursor_opacity+=1;
        }
        else if(!cursor_fade_direction && cursor_opacity>0){
            cursor_opacity-=1;
        }
    }
}

void Engine_Interface::animate_gui_selector_chasers(){
    Engine_Rect object_pos=get_gui_selected_object_pos();

    if(object_pos.x!=-1){
        double distance_to_cover=object_pos.w*2+object_pos.h*2;
        double move_speed=distance_to_cover/(5760.0/Engine::UPDATE_RATE);

        for(int i=0;i<gui_selector_chasers.size();i++){
            if(gui_selector_chasers[i].x>(double)object_pos.x && gui_selector_chasers[i].x<(double)object_pos.x+(double)object_pos.w && gui_selector_chasers[i].y==(double)object_pos.y){
                gui_selector_chasers[i].x+=move_speed;

                if(gui_selector_chasers[i].x>=(double)object_pos.x+(double)object_pos.w){
                    gui_selector_chasers[i].x=(double)object_pos.x+(double)object_pos.w;
                    gui_selector_chasers[i].y+=move_speed;
                }
            }
            else if(gui_selector_chasers[i].x>(double)object_pos.x && gui_selector_chasers[i].x<(double)object_pos.x+(double)object_pos.w && gui_selector_chasers[i].y==(double)object_pos.y+(double)object_pos.h){
                gui_selector_chasers[i].x-=move_speed;

                if(gui_selector_chasers[i].x<=(double)object_pos.x){
                    gui_selector_chasers[i].x=(double)object_pos.x;
                    gui_selector_chasers[i].y-=move_speed;
                }
            }
            else if(gui_selector_chasers[i].y>(double)object_pos.y && gui_selector_chasers[i].y<(double)object_pos.y+(double)object_pos.h && gui_selector_chasers[i].x==(double)object_pos.x){
                gui_selector_chasers[i].y-=move_speed;

                if(gui_selector_chasers[i].y<=(double)object_pos.y){
                    gui_selector_chasers[i].y=(double)object_pos.y;
                    gui_selector_chasers[i].x+=move_speed;
                }
            }
            else if(gui_selector_chasers[i].y>(double)object_pos.y && gui_selector_chasers[i].y<(double)object_pos.y+(double)object_pos.h && gui_selector_chasers[i].x==(double)object_pos.x+(double)object_pos.w){
                gui_selector_chasers[i].y+=move_speed;

                if(gui_selector_chasers[i].y>=(double)object_pos.y+(double)object_pos.h){
                    gui_selector_chasers[i].y=(double)object_pos.y+(double)object_pos.h;
                    gui_selector_chasers[i].x-=move_speed;
                }
            }
            else{
                if(i==0){
                    gui_selector_chasers[i].x=(double)object_pos.x+move_speed;
                    gui_selector_chasers[i].y=(double)object_pos.y;
                }
                else if(i==1){
                    gui_selector_chasers[i].x=(double)object_pos.x+(double)object_pos.w-move_speed;
                    gui_selector_chasers[i].y=(double)object_pos.y+(double)object_pos.h;
                }
            }
        }
    }
}

void Engine_Interface::render_gui_selector(){
    Engine_Rect object_pos=get_gui_selected_object_pos();

    if(object_pos.x!=-1){
        if(gui_selector_style=="standard" || gui_selector_style=="corners" || gui_selector_style=="chasers"){
            double thickness=Engine_Data::gui_border_thickness;
            double offset=Engine_Data::gui_border_thickness;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                thickness+=2.0;
                offset+=2.0;
            }

            Render::render_rectangle_empty(object_pos.x-offset,object_pos.y-offset,object_pos.w+offset*2.0,object_pos.h+offset*2.0,1.0,Engine::current_color_theme()->gui_selector_1,thickness);
        }

        if(gui_selector_style=="corners"){
            double corner_size=Engine_Data::gui_border_thickness+4.0;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                corner_size+=2.0;
            }

            Render::render_rectangle(object_pos.x-corner_size/2.0+1,object_pos.y-corner_size/2.0+1,corner_size,corner_size,1.0,Engine::current_color_theme()->gui_selector_2);
            Render::render_rectangle(object_pos.x+object_pos.w-corner_size/2.0-1,object_pos.y-corner_size/2.0+1,corner_size,corner_size,1.0,Engine::current_color_theme()->gui_selector_2);
            Render::render_rectangle(object_pos.x-corner_size/2.0+1,object_pos.y+object_pos.h-corner_size/2.0-1,corner_size,corner_size,1.0,Engine::current_color_theme()->gui_selector_2);
            Render::render_rectangle(object_pos.x+object_pos.w-corner_size/2.0-1,object_pos.y+object_pos.h-corner_size/2.0-1,corner_size,corner_size,1.0,Engine::current_color_theme()->gui_selector_2);
        }

        if(gui_selector_style=="underline"){
            double thickness=Engine_Data::gui_border_thickness+1.0;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                thickness+=3.0;
            }

            for(int i=0;i<thickness;i++){
                Render::render_line(object_pos.x,object_pos.y+object_pos.h+i,object_pos.x+object_pos.w-1,object_pos.y+object_pos.h+i,1.0,Engine::current_color_theme()->gui_selector_1);
            }
        }

        if(gui_selector_style=="chasers"){
            double chaser_size=Engine_Data::gui_border_thickness+4.0;

            if(mutable_info_selected() && !is_console_selected() && !is_chat_selected()){
                chaser_size+=2.0;
            }

            for(int i=0;i<gui_selector_chasers.size();i++){
                Render::render_rectangle(gui_selector_chasers[i].x-chaser_size/2.0,gui_selector_chasers[i].y-chaser_size/2.0,chaser_size,chaser_size,1.0,Engine::current_color_theme()->gui_selector_2);
            }
        }
    }
}

void Engine_Interface::render_small_text_inputter(){
    Render::render_rectangle(0,0,Game_Window::SCREEN_WIDTH,Game_Window::SCREEN_HEIGHT,0.75,Engine::current_color_theme()->window_border);
    Render::render_rectangle(Engine_Data::window_border_thickness,Engine_Data::window_border_thickness,Game_Window::SCREEN_WIDTH-Engine_Data::window_border_thickness*2.0,
                     Game_Window::SCREEN_HEIGHT-Engine_Data::window_border_thickness*2.0,0.75,Engine::current_color_theme()->window_background);

    if(mutable_info_selected()){
        Bitmap_Font* font=Object_Manager::get_font("small");

        font->show(Engine_Data::window_border_thickness+2.0,Engine_Data::window_border_thickness+2.0,ptr_mutable_info->get_cursor_line(),Engine::current_color_theme()->window_font);
    }

    Bitmap_Font* font=Object_Manager::get_font("standard");

    double buttons_start_y=Engine_Data::window_border_thickness+2.0+font->spacing_y*2.0;

    double x_offset=((double)Game_Window::SCREEN_WIDTH-font->get_letter_width()*13.0)/2.0+Engine_Data::window_border_thickness;
    double offset_y=buttons_start_y;

    for(int i=0;i<13;i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_upper[i],Engine::current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=13;i<characters_upper.size();i++){
        font->show(x_offset+(i-13)*font->get_letter_width(),offset_y,characters_upper[i],Engine::current_color_theme()->window_font);
    }

    offset_y+=font->get_letter_height();
    for(int i=0;i<13;i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_lower[i],Engine::current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=13;i<characters_lower.size();i++){
        font->show(x_offset+(i-13)*font->get_letter_width(),offset_y,characters_lower[i],Engine::current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();

    for(int i=0;i<characters_numbers.size();i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_numbers[i],Engine::current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();

    for(int i=0;i<13;i++){
        font->show(x_offset+i*font->get_letter_width(),offset_y,characters_symbols[i],Engine::current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=13;i<26;i++){
        font->show(x_offset+(i-13)*font->get_letter_width(),offset_y,characters_symbols[i],Engine::current_color_theme()->window_font);
    }
    offset_y+=font->get_letter_height();
    for(int i=26;i<characters_symbols.size();i++){
        font->show(x_offset+(i-26)*font->get_letter_width(),offset_y,characters_symbols[i],Engine::current_color_theme()->window_font);
    }

    text_selector.render(x_offset+text_entry_small_selector.x*font->get_letter_width()-(text_selector.get_width()-font->get_letter_width())/2.0,buttons_start_y+text_entry_small_selector.y*font->get_letter_height()-(text_selector.get_height()-font->get_letter_height())/2.0);
}

void Engine_Interface::render_text_inputter(){
    vector<string>* characters=get_text_input_character_set();

    int selected_chunk=get_text_input_selected_chunk();

    double outer_radius=Game_Window::SCREEN_WIDTH/5;
    double outer_center_y=outer_radius+10;

    //The topmost instructions' y position.
    int words_y=outer_center_y+outer_radius+10;

    int x_adjust=0;
    if(mutable_info_selected()){
        Engine_Rect object_pos;

        if(is_console_selected()){
            object_pos.x=console.info_input.x;
            object_pos.y=console.info_input.y;
            object_pos.w=console.info_input.w;
            object_pos.h=console.info_input.h;
        }
        else if(is_chat_selected()){
            object_pos.x=chat.info_input.x;
            object_pos.y=chat.info_input.y;
            object_pos.w=chat.info_input.w;
            object_pos.h=chat.info_input.h;
        }
        else{
            object_pos=get_gui_selected_object_pos();
        }

        if(object_pos.x!=-1){
            if(object_pos.x<=Game_Window::SCREEN_WIDTH/2){
                x_adjust=Game_Window::SCREEN_WIDTH/2;
            }

            if(object_pos.y<=Game_Window::SCREEN_HEIGHT/2){
                outer_center_y=Game_Window::SCREEN_HEIGHT-outer_radius-10;
                words_y=outer_center_y-outer_radius-120;
            }
        }
    }

    double outer_center_x=x_adjust+Game_Window::SCREEN_WIDTH/4;

    Bitmap_Font* font=Object_Manager::get_font("standard");

    Render::render_rectangle(x_adjust,0,Game_Window::SCREEN_WIDTH/2,Game_Window::SCREEN_HEIGHT,0.75,Engine::current_color_theme()->window_background);
    Render::render_rectangle_empty(x_adjust,0,Game_Window::SCREEN_WIDTH/2,Game_Window::SCREEN_HEIGHT,0.75,Engine::current_color_theme()->window_border,2.0);
    Render::render_circle(outer_center_x+4,outer_center_y+4,outer_radius,1.0,"ui_black");
    Render::render_circle(outer_center_x,outer_center_y,outer_radius,1.0,Engine::current_color_theme()->window_border);

    Render::render_circle(outer_center_x,outer_center_y,16.0,1.0,"ui_gray");
    font->show(outer_center_x-12.0,outer_center_y-8.0,"LS","ui_white");

    string text="";

    text="(Press for Return)";
    font->show(x_adjust+(Game_Window::SCREEN_WIDTH/2-text.length()*font->spacing_x)/2,outer_center_y-8.0+font->spacing_y,text,"ui_white");

    text="[LB] Backspace";
    font->show(x_adjust+100,words_y,text,Engine::current_color_theme()->window_font);
    text="Space [RB]";
    font->show(x_adjust+Game_Window::SCREEN_WIDTH/2-text.length()*font->spacing_x-100,words_y,text,Engine::current_color_theme()->window_font);

    text="(LT) Caps";
    font->show(x_adjust+100,words_y+30,text,Engine::current_color_theme()->window_font);
    text="Numbers (RT)";
    font->show(x_adjust+Game_Window::SCREEN_WIDTH/2-text.length()*font->spacing_x-100,words_y+30,text,Engine::current_color_theme()->window_font);

    if(selected_chunk==-1){
        if(is_console_selected()){
            text="(A) Auto-complete";
            font->show(x_adjust+(Game_Window::SCREEN_WIDTH/2-text.length()*font->spacing_x)/2,words_y+60,text,Engine::current_color_theme()->window_font);
        }

        text="(B) Back/Done";
        font->show(x_adjust+(Game_Window::SCREEN_WIDTH/2-text.length()*font->spacing_x)/2,words_y+90,text,Engine::current_color_theme()->window_font);
    }

    int character_chunk=0;
    int character=0;

    for(double angle=-90.0;angle<270.0;angle+=45.0){
        double mid_radius=outer_radius*0.75;
        double inner_radius=50.0;

        double inner_center_x=outer_center_x+mid_radius*cos(Math::degrees_to_radians(angle));
        double inner_center_y=outer_center_y+mid_radius*sin(Math::degrees_to_radians(angle));

        string outer_circle_color=Engine::current_color_theme()->window_background;
        if(character_chunk==selected_chunk){
            outer_circle_color=Engine::current_color_theme()->window_title_bar;
        }

        Render::render_circle(inner_center_x,inner_center_y,inner_radius,1.0,outer_circle_color);

        for(double letter_angle=-180.0;letter_angle<180.0;letter_angle+=90.0){
            double letter_circle_radius=inner_radius*0.75;
            double letter_radius=16.0;

            string circle_color="text_input_blue";
            if(letter_angle==-180.0){
                circle_color="text_input_blue";
            }
            else if(letter_angle==-90.0){
                circle_color="text_input_yellow";
            }
            else if(letter_angle==0.0){
                circle_color="text_input_red";
            }
            else if(letter_angle==90.0){
                circle_color="text_input_green";
            }

            double letter_center_x=inner_center_x+letter_circle_radius*cos(Math::degrees_to_radians(letter_angle));
            double letter_center_y=inner_center_y+letter_circle_radius*sin(Math::degrees_to_radians(letter_angle));

            string font_color=Engine::current_color_theme()->window_font;

            if(character_chunk==selected_chunk){
                font_color="ui_white";

                Render::render_circle(letter_center_x,letter_center_y,letter_radius,1.0,circle_color);
            }

            if(character<characters->size()){
                font->show(letter_center_x-8.0,letter_center_y-8.0,characters->at(character),font_color);
            }

            character++;
        }

        character_chunk++;
    }
}

void Engine_Interface::render_text_editing(){
    if(mutable_info_selected()){
        Bitmap_Font* font=Object_Manager::get_font("small");

        string text=ptr_mutable_info->get_cursor_line();

        Render::render_rectangle(0.0,0.0,Game_Window::SCREEN_WIDTH,font->spacing_y+Engine_Data::window_border_thickness*2.0,0.75,Engine::current_color_theme()->window_border);
        Render::render_rectangle(Engine_Data::window_border_thickness,Engine_Data::window_border_thickness,Game_Window::SCREEN_WIDTH-Engine_Data::window_border_thickness*2.0,
                         font->spacing_y,0.75,Engine::current_color_theme()->window_background);

        font->show((Game_Window::SCREEN_WIDTH-(text.length()*font->spacing_x))/2.0,Engine_Data::window_border_thickness,text,Engine::current_color_theme()->window_font);
    }
}

void Engine_Interface::render(int render_rate,double ms_per_frame,int logic_frame_rate){
    if(!hide_gui){
        if(Game_Manager::in_progress && Game_Manager::paused){
            render_pause();
        }

        vector<int> closed_windows;
        for(int i=0;i<windows.size();i++){
            if(!is_window_open(&windows[i])){
                closed_windows.push_back(i);
            }
        }

        for(int i=0;i<closed_windows.size();i++){
            windows[closed_windows[i]].render();
        }

        for(int i=window_z_order.size()-1;i>=0;i--){
            window_z_order[i]->render();
        }

        render_gui_selector();

        console.render();
        chat.render();

        tooltip.render();

        if(toasts.size()>0){
            toasts[0].render();
        }

        if(mutable_info_selected() && gui_mode=="controller" && !allow_screen_keyboard()){
            if(!Engine_Data::controller_text_entry_small){
                render_text_inputter();
            }
            else{
                render_small_text_inputter();
            }
        }

        if(mutable_info_selected() && allow_screen_keyboard() && Game_Window::is_screen_keyboard_shown()){
            render_text_editing();
        }

        Controller_Manager::render_touch_controller();

        if(Options::dev){
            render_dev_info();
        }

        if(Options::fps){
            render_fps(render_rate,ms_per_frame,logic_frame_rate);
        }

        if(gui_mode=="mouse" && (Engine_Data::cursor_render_always || is_any_window_open() || console.on)){
            int mouse_x=0;
            int mouse_y=0;
            get_mouse_state(&mouse_x,&mouse_y);

            if(!mouse_over){
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
