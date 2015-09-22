/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "engine_interface.h"
#include "world.h"
#include "version.h"
#include "game_options.h"

#include <engine_version.h>
#include <strings.h>
#include <log.h>
#include <directories.h>
#include <options.h>
#include <sorting.h>
#include <music_manager.h>

#include <boost/algorithm/string.hpp>
#include <boost/crc.hpp>

using namespace std;

Version_Series::Version_Series(int get_major_1,int get_minor_1,int get_micro_1,int get_major_2,int get_minor_2,int get_micro_2){
    major_1=get_major_1;
    minor_1=get_minor_1;
    micro_1=get_micro_1;

    major_2=get_major_2;
    minor_2=get_minor_2;
    micro_2=get_micro_2;
}

string Engine_Interface::get_version(){
    return Strings::num_to_string(Version::get_major())+"."+Strings::num_to_string(Version::get_minor())+"."+Strings::num_to_string(Version::get_micro());
}

string Engine_Interface::get_build_date(){
    string year=Version::get_year();
    string month=Version::get_month();
    string day=Version::get_day();

    return year+"-"+month+"-"+day;
}

string Engine_Interface::get_engine_version(){
    return Engine_Version::get_engine_version();
}

string Engine_Interface::get_engine_date(){
    return Engine_Version::get_engine_date();
}

string Engine_Interface::get_checksum(){
    string checksum="";
    string checksum_data="";

    vector<string> file_list;

    for(File_IO_Directory_Iterator it("data");it.evaluate();it.iterate()){
        //If the file is not a directory.
        if(it.is_regular_file()){
            string file_path=it.get_full_path();

            file_list.push_back(file_path);
        }
    }

    Sorting::quick_sort(file_list);

    for(int i=0;i<file_list.size();i++){
        File_IO_Load load(file_list[i]);

        if(load.is_opened()){
            checksum_data+=load.get_data();
        }
        else{
            Log::add_error("Error loading file for checksum calculation: '"+file_list[i]+"'");
        }
    }

    if(checksum_data.length()>0){
        boost::crc_32_type result;
        result.process_bytes(checksum_data.data(),checksum_data.length());

        checksum=Strings::num_to_string((uint32_t)result.checksum());
    }

    return checksum;
}

bool Engine_Interface::save_save_location(){
    stringstream save("");

    save<<"<save_location>\n";

    save<<"\tsave_location:"<<Options::save_location<<"\n";

    save<<"</save_location>\n";

    if(File_IO::save_atomic("save_location.cfg",save.str())){
        Directories::save_location_loaded=true;

        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_save_location(){
    File_IO_Load load("save_location.cfg");

    if(load.is_opened()){
        bool multi_line_comment=false;

        //As long as we haven't reached the end of the file.
        while(!load.eof()){
            string line="";

            //The option strings used in the file.

            string str_save_location="save_location:";

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

            //Load data based on the line.

            //save_location
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_save_location)){
                //Clear the data name.
                line.erase(0,str_save_location.length());

                Options::save_location=line;

                Directories::save_location_loaded=true;
            }

            //If the line ends the save location data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</save_location>")){
                break;
            }
        }
    }
    else{
        if(!save_save_location()){
            return false;
        }
    }

    return true;
}

int Engine_Interface::version_compare(int major_1,int minor_1,int micro_1,int major_2,int minor_2,int micro_2){
    if(major_1==major_2){
        if(minor_1==minor_2){
            if(micro_1==micro_2){
                return 0;
            }
            else if(micro_1<micro_2){
                return -1;
            }
            else if(micro_1>micro_2){
                return 1;
            }
        }
        else if(minor_1<minor_2){
            return -1;
        }
        else if(minor_1>minor_2){
            return 1;
        }
    }
    else if(major_1<major_2){
        return -1;
    }
    else if(major_1>major_2){
        return 1;
    }
}

int Engine_Interface::which_version_series(vector<Version_Series>* version_series,int major,int minor,int micro){
    for(int i=0;i<version_series->size();i++){
        if(version_compare(major,minor,micro,version_series->at(i).major_1,version_series->at(i).minor_1,version_series->at(i).micro_1)>=0 &&
           version_compare(major,minor,micro,version_series->at(i).major_2,version_series->at(i).minor_2,version_series->at(i).micro_2)<=0){
            return i;
        }
    }

    return -1;
}

bool Engine_Interface::version_compatible(string name_to_check){
    int current_major=Version::get_major();
    int current_minor=Version::get_minor();
    int current_micro=Version::get_micro();

    int major=0;
    int minor=0;
    int micro=0;

    vector<string> version_strings;
    boost::algorithm::split(version_strings,option_version,boost::algorithm::is_any_of("."));

    major=Strings::string_to_long(version_strings[0]);
    minor=Strings::string_to_long(version_strings[1]);
    micro=Strings::string_to_long(version_strings[2]);

    //Version series are defined by a start version and an end version.
    //The start version must be less than or equal to the end version.
    vector<Version_Series> version_series;

    version_series.push_back(Version_Series(0,0,0,0,0,1));

    if(which_version_series(&version_series,major,minor,micro)!=which_version_series(&version_series,current_major,current_minor,current_micro)){
        string error_message="Version incompatibility! Save data was created with version "+option_version;
        error_message+=". Current version is "+get_version()+".";

        Log::add_error(error_message);

        return false;
    }

    return true;
}

string Engine_Interface::get_option_value(string option){
    if(option=="cl_logic_update_rate"){
        return Strings::num_to_string(UPDATE_RATE);
    }
    else if(option=="cl_frame_rate_max"){
        return Strings::num_to_string(UPDATE_RATE_RENDER);
    }

    else if(option=="cl_screen_width"){
        return Strings::num_to_string(Options::screen_width);
    }
    else if(option=="cl_screen_height"){
        return Strings::num_to_string(Options::screen_height);
    }
    else if(option=="cl_display_number"){
        return Strings::num_to_string(Options::display_number);
    }
    else if(option=="cl_fullscreen_state"){
        return Strings::bool_to_string(Options::fullscreen);
    }
    else if(option=="cl_fullscreen_mode"){
        return Options::fullscreen_mode;
    }

    else if(option=="cl_vsync"){
        return Strings::bool_to_string(Options::vsync);
    }
    else if(option=="cl_accelerometer_controller"){
        return Strings::bool_to_string(Options::accelerometer_controller);
    }
    else if(option=="cl_touch_controller_state"){
        return Strings::bool_to_string(Options::touch_controller_state);
    }
    else if(option=="cl_touch_controller_opacity"){
        return Strings::num_to_string(Options::touch_controller_opacity);
    }
    else if(option=="cl_font_shadows"){
        return Strings::bool_to_string(Options::font_shadows);
    }
    else if(option=="cl_screen_keyboard"){
        return Strings::bool_to_string(Options::screen_keyboard);
    }
    else if(option=="cl_hw_cursor"){
        return Strings::bool_to_string(Options::hw_cursor);
    }
    else if(option=="cl_bind_cursor"){
        return Strings::bool_to_string(Options::bind_cursor);
    }

    else if(option=="cl_fps"){
        return Strings::bool_to_string(Options::fps);
    }
    else if(option=="cl_dev"){
        return Strings::bool_to_string(Options::dev);
    }
    else if(option=="cl_volume_global"){
        return Strings::num_to_string(Options::volume_global);
    }
    else if(option=="cl_volume_sound"){
        return Strings::num_to_string(Options::volume_sound);
    }
    else if(option=="cl_volume_music"){
        return Strings::num_to_string(Options::volume_music);
    }
    else if(option=="cl_mute_global"){
        return Strings::bool_to_string(Options::mute_global);
    }
    else if(option=="cl_mute_sound"){
        return Strings::bool_to_string(Options::mute_sound);
    }
    else if(option=="cl_mute_music"){
        return Strings::bool_to_string(Options::mute_music);
    }

    for(int i=0;i<game_options.size();i++){
        if(option==game_options[i].name){
            return Game_Options::get_value(game_options[i].name);
        }
    }

    return "";
}

string Engine_Interface::get_option_description(string option){
    if(option=="cl_logic_update_rate"){
        return "the game logic update rate\n - changes to this option won't persist through a game restart\n - cannot be changed during networked play";
    }
    else if(option=="cl_frame_rate_max"){
        return "the maximum frame rate\n - changes to this option won't persist through a game restart";
    }

    else if(option=="cl_screen_width"){
        return "the width of the screen in pixels\n - changes to this option are only applied when the UI is reloaded with \"reload\"";
    }
    else if(option=="cl_screen_height"){
        return "the height of the screen in pixels\n - changes to this option are only applied when the UI is reloaded with \"reload\"";
    }
    else if(option=="cl_display_number"){
        return "the display number that the game's window should be placed on\n - changes to this option are only applied when the UI is reloaded with \"reload\"";
    }
    else if(option=="cl_fullscreen_state"){
        return "enable or disable the fullscreen status of the window\n - changes to this option are only applied when the UI is reloaded with \"reload\"";
    }
    else if(option=="cl_fullscreen_mode"){
        return "the mode used when the window is made fullscreen\n - changes to this option are only applied when the UI is reloaded with \"reload\"\n - valid values: \"standard\", \"desktop\", \"windowed\"";
    }

    else if(option=="cl_vsync"){
        return "enable vertical synchronization\n - changes to this option are only applied when the game is restarted";
    }
    else if(option=="cl_accelerometer_controller"){
        return "allow the Android accelerometer to be used as a game controller\n - changes to this option are only applied when the game is restarted";
    }
    else if(option=="cl_touch_controller_state"){
        return "enable the virtual touchscreen controller";
    }
    else if(option=="cl_touch_controller_opacity"){
        return "the opacity of the touchscreen controller\n - valid values: 0.0 - 1.0";
    }
    else if(option=="cl_font_shadows"){
        return "enable font shadows";
    }
    else if(option=="cl_screen_keyboard"){
        return "enable the screen keyboard";
    }
    else if(option=="cl_hw_cursor"){
        return "enable hardware cursor";
    }
    else if(option=="cl_bind_cursor"){
        return "bind the cursor to the window";
    }

    else if(option=="cl_fps"){
        return "show FPS in-game and in the window's titlebar";
    }
    else if(option=="cl_dev"){
        return "enable developer mode";
    }
    else if(option=="cl_volume_global"){
        return "the global volume modifier\n - valid values: 0.0 - 1.0";
    }
    else if(option=="cl_volume_sound"){
        return "the volume modifier for sound effects\n - valid values: 0.0 - 1.0";
    }
    else if(option=="cl_volume_music"){
        return "the volume modifier for music\n - valid values: 0.0 - 1.0";
    }
    else if(option=="cl_mute_global"){
        return "mute all audio";
    }
    else if(option=="cl_mute_sound"){
        return "mute sound effects";
    }
    else if(option=="cl_mute_music"){
        return "mute music";
    }

    for(int i=0;i<game_options.size();i++){
        if(option==game_options[i].name){
            return game_options[i].description;
        }
    }
}

void Engine_Interface::change_option(string option,string new_value){
    if(option=="cl_logic_update_rate"){
        if(network.status=="off"){
            set_logic_update_rate(Strings::string_to_double(new_value));
        }
    }
    else if(option=="cl_frame_rate_max"){
        set_render_update_rate(Strings::string_to_double(new_value));
    }

    else if(option=="cl_screen_width"){
        Options::screen_width=Strings::string_to_long(new_value);
    }
    else if(option=="cl_screen_height"){
        Options::screen_height=Strings::string_to_long(new_value);
    }
    else if(option=="cl_display_number"){
        Options::display_number=Strings::string_to_long(new_value);
    }
    else if(option=="cl_fullscreen_state"){
        Options::fullscreen=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_fullscreen_mode"){
        Options::fullscreen_mode=new_value;
    }

    else if(option=="cl_vsync"){
        Options::vsync=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_accelerometer_controller"){
        Options::accelerometer_controller=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_touch_controller_state"){
        Options::touch_controller_state=Strings::string_to_bool(new_value);

        if(Options::touch_controller_state && SDL_GetNumTouchDevices()>0){
            touch_controls=true;
        }
    }
    else if(option=="cl_touch_controller_opacity"){
        Options::touch_controller_opacity=Strings::string_to_double(new_value);
    }
    else if(option=="cl_font_shadows"){
        Options::font_shadows=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_screen_keyboard"){
        Options::screen_keyboard=Strings::string_to_bool(new_value);

        if(!Options::screen_keyboard && SDL_IsScreenKeyboardShown(main_window.screen)){
            SDL_StopTextInput();
        }
    }
    else if(option=="cl_hw_cursor"){
        Options::hw_cursor=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_bind_cursor"){
        Options::bind_cursor=Strings::string_to_bool(new_value);

        if(Options::bind_cursor){
            SDL_SetWindowGrab(main_window.screen,SDL_TRUE);
        }
        else{
            SDL_SetWindowGrab(main_window.screen,SDL_FALSE);
        }
    }

    else if(option=="cl_fps"){
        Options::fps=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_dev"){
        Options::dev=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_volume_global"){
        Options::volume_global=Strings::string_to_double(new_value);

        Music_Manager::set_track_volumes();
    }
    else if(option=="cl_volume_sound"){
        Options::volume_sound=Strings::string_to_double(new_value);
    }
    else if(option=="cl_volume_music"){
        Options::volume_music=Strings::string_to_double(new_value);

        Music_Manager::set_track_volumes();
    }
    else if(option=="cl_mute_global"){
        Options::mute_global=Strings::string_to_bool(new_value);

        Music_Manager::set_track_volumes();
    }
    else if(option=="cl_mute_sound"){
        Options::mute_sound=Strings::string_to_bool(new_value);
    }
    else if(option=="cl_mute_music"){
        Options::mute_music=Strings::string_to_bool(new_value);

        Music_Manager::set_track_volumes();
    }

    else{
        bool game_option=false;

        for(int i=0;i<game_options.size();i++){
            if(option==game_options[i].name){
                Game_Options::set_value(game_options[i].name,new_value);

                game_option=true;

                break;
            }
        }

        if(!game_option){
            return;
        }
    }

    save_options();
}

void Engine_Interface::apply_options(const string& cl_effect_limit,const string& cl_screen_shake,const string& cl_rumble){
    change_option("cl_effect_limit",cl_effect_limit);
    change_option("cl_screen_shake",cl_screen_shake);
    change_option("cl_rumble",cl_rumble);
}

void Engine_Interface::apply_options_graphics(const string& cl_screen_width,const string& cl_screen_height,const string& cl_display_number,
                                              const string& cl_fullscreen_state,const string& cl_fullscreen_mode,const string& cl_vsync,
                                              const string& cl_fps,const string& cl_hw_cursor,const string& cl_font_shadows){
    string old_screen_width=get_option_value("cl_screen_width");
    string old_screen_height=get_option_value("cl_screen_height");
    string old_display_number=get_option_value("cl_display_number");
    string old_fullscreen_state=get_option_value("cl_fullscreen_state");
    string old_fullscreen_mode=get_option_value("cl_fullscreen_mode");

    change_option("cl_screen_width",cl_screen_width);
    change_option("cl_screen_height",cl_screen_height);
    change_option("cl_display_number",cl_display_number);
    change_option("cl_fullscreen_state",cl_fullscreen_state);
    change_option("cl_fullscreen_mode",cl_fullscreen_mode);
    change_option("cl_vsync",cl_vsync);
    change_option("cl_fps",cl_fps);
    change_option("cl_hw_cursor",cl_hw_cursor);
    change_option("cl_font_shadows",cl_font_shadows);

    if(old_screen_width!=cl_screen_width || old_screen_height!=cl_screen_height || old_display_number!=cl_display_number ||
       old_fullscreen_state!=cl_fullscreen_state || old_fullscreen_mode!=cl_fullscreen_mode){
        reload();
    }
}

void Engine_Interface::apply_options_audio(const string& cl_volume_global,const string& cl_mute_global,
                                           const string& cl_volume_sound,const string& cl_mute_sound,
                                           const string& cl_volume_music,const string& cl_mute_music){
    double volume_global=1.0;
    if(cl_volume_global=="Low"){
        volume_global=0.25;
    }
    else if(cl_volume_global=="Medium"){
        volume_global=0.5;
    }
    change_option("cl_volume_global",Strings::num_to_string(volume_global));
    change_option("cl_mute_global",cl_mute_global);

    double volume_sound=1.0;
    if(cl_volume_sound=="Low"){
        volume_sound=0.25;
    }
    else if(cl_volume_sound=="Medium"){
        volume_sound=0.5;
    }
    change_option("cl_volume_sound",Strings::num_to_string(volume_sound));
    change_option("cl_mute_sound",cl_mute_sound);

    double volume_music=1.0;
    if(cl_volume_music=="Low"){
        volume_music=0.25;
    }
    else if(cl_volume_music=="Medium"){
        volume_music=0.5;
    }
    change_option("cl_volume_music",Strings::num_to_string(volume_music));
    change_option("cl_mute_music",cl_mute_music);
}

void Engine_Interface::apply_options_input(const string& cl_bind_cursor,const string& cl_screen_keyboard,const string& cl_accelerometer_controller,
                                           const string& cl_touch_controller_state,const string& cl_touch_controller_opacity){
    change_option("cl_bind_cursor",cl_bind_cursor);
    change_option("cl_screen_keyboard",cl_screen_keyboard);
    change_option("cl_accelerometer_controller",cl_accelerometer_controller);
    change_option("cl_touch_controller_state",cl_touch_controller_state);

    double tc_opacity=1.0;
    if(cl_touch_controller_opacity=="Low"){
        tc_opacity=0.25;
    }
    else if(cl_touch_controller_opacity=="Medium"){
        tc_opacity=0.5;
    }
    change_option("cl_touch_controller_opacity",Strings::num_to_string(tc_opacity));
}

void Engine_Interface::apply_options_network(const string& cl_name,const string& cl_chat_timestamps){
    change_option("cl_name",cl_name);
    change_option("cl_chat_timestamps",cl_chat_timestamps);
}

bool Engine_Interface::save_options(){
    stringstream save("");

    save<<"<options>\n";

    save<<"\tversion:"<<option_version<<"\n";

    save<<"\n";

    save<<"\tscreen_width:"<<Options::screen_width<<"\n";
    save<<"\tscreen_height:"<<Options::screen_height<<"\n";
    save<<"\tdisplay_number:"<<Options::display_number<<"\n";
    save<<"\tfullscreen_state:"<<Strings::bool_to_string(Options::fullscreen)<<"\n";
    save<<"\tfullscreen_mode:"<<Options::fullscreen_mode<<"\n";

    save<<"\n";

    save<<"\tvsync:"<<Strings::bool_to_string(Options::vsync)<<"\n";
    save<<"\taccelerometer_controller:"<<Strings::bool_to_string(Options::accelerometer_controller)<<"\n";
    save<<"\ttouch_controller_state:"<<Strings::bool_to_string(Options::touch_controller_state)<<"\n";
    save<<"\ttouch_controller_opacity:"<<Strings::num_to_string(Options::touch_controller_opacity)<<"\n";
    save<<"\tfont_shadows:"<<Strings::bool_to_string(Options::font_shadows)<<"\n";
    save<<"\tscreen_keyboard:"<<Strings::bool_to_string(Options::screen_keyboard)<<"\n";
    save<<"\thw_cursor:"<<Strings::bool_to_string(Options::hw_cursor)<<"\n";
    save<<"\tbind_cursor:"<<Strings::bool_to_string(Options::bind_cursor)<<"\n";

    save<<"\n";

    save<<"\tfps:"<<Strings::bool_to_string(Options::fps)<<"\n";
    save<<"\tdev:"<<Strings::bool_to_string(Options::dev)<<"\n";
    save<<"\tvolume_global:"<<Options::volume_global<<"\n";
    save<<"\tvolume_sound:"<<Options::volume_sound<<"\n";
    save<<"\tvolume_music:"<<Options::volume_music<<"\n";
    save<<"\tmute_global:"<<Strings::bool_to_string(Options::mute_global)<<"\n";
    save<<"\tmute_sound:"<<Strings::bool_to_string(Options::mute_sound)<<"\n";
    save<<"\tmute_music:"<<Strings::bool_to_string(Options::mute_music)<<"\n";

    save<<"\n";

    for(int i=0;i<game_options.size();i++){
        save<<"\t"<<game_options[i].name<<":"<<Game_Options::get_value(game_options[i].name)<<"\n";
    }

    save<<"</options>\n";

    if(File_IO::save_atomic(Directories::get_save_directory()+"options.cfg",save.str(),true)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_options(){
    File_IO_Load load(Directories::get_save_directory()+"options.cfg");

    if(load.is_opened()){
        bool multi_line_comment=false;

        //As long as we haven't reached the end of the file.
        while(!load.eof()){
            string line="";

            //The option strings used in the file.

            string str_version="version:";
            string str_screen_width="screen_width:";
            string str_screen_height="screen_height:";
            string str_display_number="display_number:";
            string str_fullscreen="fullscreen_state:";
            string str_fullscreen_mode="fullscreen_mode:";

            string str_vsync="vsync:";
            string str_accelerometer_controller="accelerometer_controller:";
            string str_touch_controller_state="touch_controller_state:";
            string str_touch_controller_opacity="touch_controller_opacity:";
            string str_font_shadows="font_shadows:";
            string str_screen_keyboard="screen_keyboard:";
            string str_hw_cursor="hw_cursor:";
            string str_bind_cursor="bind_cursor:";

            string str_fps="fps:";
            string str_dev="dev:";
            string str_volume_global="volume_global:";
            string str_volume_sound="volume_sound:";
            string str_volume_music="volume_music:";
            string str_mute_global="mute_global:";
            string str_mute_sound="mute_sound:";
            string str_mute_music="mute_music:";

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

            //Load data based on the line.

            //version
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_version)){
                //Clear the data name.
                line.erase(0,str_version.length());

                option_version=line;
            }
            //screen_width
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_screen_width)){
                //Clear the data name.
                line.erase(0,str_screen_width.length());

                Options::screen_width=Strings::string_to_long(line);
            }
            //screen_height
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_screen_height)){
                //Clear the data name.
                line.erase(0,str_screen_height.length());

                Options::screen_height=Strings::string_to_long(line);
            }
            //display_number
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_display_number)){
                //Clear the data name.
                line.erase(0,str_display_number.length());

                Options::display_number=Strings::string_to_long(line);
            }
            //fullscreen
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fullscreen)){
                //Clear the data name.
                line.erase(0,str_fullscreen.length());

                Options::fullscreen=Strings::string_to_bool(line);
            }
            //fullscreen_mode
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fullscreen_mode)){
                //Clear the data name.
                line.erase(0,str_fullscreen_mode.length());

                Options::fullscreen_mode=line;
            }

            //vsync
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_vsync)){
                //Clear the data name.
                line.erase(0,str_vsync.length());

                Options::vsync=Strings::string_to_bool(line);
            }
            //accelerometer_controller
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_accelerometer_controller)){
                //Clear the data name.
                line.erase(0,str_accelerometer_controller.length());

                Options::accelerometer_controller=Strings::string_to_bool(line);
            }
            //touch_controller_state
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_state)){
                //Clear the data name.
                line.erase(0,str_touch_controller_state.length());

                Options::touch_controller_state=Strings::string_to_bool(line);
            }
            //touch_controller_opacity
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_touch_controller_opacity)){
                //Clear the data name.
                line.erase(0,str_touch_controller_opacity.length());

                Options::touch_controller_opacity=Strings::string_to_double(line);
            }
            //font_shadows
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_font_shadows)){
                //Clear the data name.
                line.erase(0,str_font_shadows.length());

                Options::font_shadows=Strings::string_to_bool(line);
            }
            //screen_keyboard
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_screen_keyboard)){
                //Clear the data name.
                line.erase(0,str_screen_keyboard.length());

                Options::screen_keyboard=Strings::string_to_bool(line);
            }
            //hw_cursor
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_hw_cursor)){
                //Clear the data name.
                line.erase(0,str_hw_cursor.length());

                Options::hw_cursor=Strings::string_to_bool(line);
            }
            //bind_cursor
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_bind_cursor)){
                //Clear the data name.
                line.erase(0,str_bind_cursor.length());

                Options::bind_cursor=Strings::string_to_bool(line);
            }

            //fps
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_fps)){
                //Clear the data name.
                line.erase(0,str_fps.length());

                Options::fps=Strings::string_to_bool(line);
            }
            //dev
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_dev)){
                //Clear the data name.
                line.erase(0,str_dev.length());

                Options::dev=Strings::string_to_bool(line);
            }
            //volume_global
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_volume_global)){
                //Clear the data name.
                line.erase(0,str_volume_global.length());

                Options::volume_global=Strings::string_to_double(line);
            }
            //volume_sound
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_volume_sound)){
                //Clear the data name.
                line.erase(0,str_volume_sound.length());

                Options::volume_sound=Strings::string_to_double(line);
            }
            //volume_music
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_volume_music)){
                //Clear the data name.
                line.erase(0,str_volume_music.length());

                Options::volume_music=Strings::string_to_double(line);
            }
            //mute_global
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mute_global)){
                //Clear the data name.
                line.erase(0,str_mute_global.length());

                Options::mute_global=Strings::string_to_bool(line);
            }
            //mute_sound
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mute_sound)){
                //Clear the data name.
                line.erase(0,str_mute_sound.length());

                Options::mute_sound=Strings::string_to_bool(line);
            }
            //mute_music
            else if(!multi_line_comment && boost::algorithm::starts_with(line,str_mute_music)){
                //Clear the data name.
                line.erase(0,str_mute_music.length());

                Options::mute_music=Strings::string_to_bool(line);
            }

            //If the line ends the options data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</options>")){
                break;
            }

            //Game Option
            else if(!multi_line_comment){
                for(int i=0;i<game_options.size();i++){
                    string str_game_option=game_options[i].name+":";

                    if(boost::algorithm::starts_with(line,str_game_option)){
                        //Clear the data name.
                        line.erase(0,str_game_option.length());

                        Game_Options::set_value(game_options[i].name,line);
                    }
                }
            }
        }
    }

    if(!save_options()){
        return false;
    }

    return true;
}

bool Engine_Interface::save_game_commands(){
    stringstream save("");

    save<<"<game_commands>\n";

    for(int i=0;i<game_commands.size();i++){
        const char* ckey=SDL_GetScancodeName(game_commands[i].key);
        const char* cbutton=SDL_GameControllerGetStringForButton(game_commands[i].button);
        const char* caxis=SDL_GameControllerGetStringForAxis(game_commands[i].axis);

        string key="";
        if(ckey!=0){
            key=ckey;
        }

        string button="";
        if(cbutton!=0){
            button=cbutton;
        }

        string axis="";
        if(caxis!=0){
            axis=caxis;
        }

        save<<"\t"<<game_commands[i].name<<","<<game_commands[i].title<<","<<Strings::add_newlines(game_commands[i].description)<<","<<Strings::bool_to_string(game_commands[i].dev)<<","<<key<<","<<button<<","<<axis<<"\n";
    }

    save<<"</game_commands>\n";

    if(File_IO::save_atomic(Directories::get_save_directory()+"game_commands.cfg",save.str(),true)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_game_commands(){
    File_IO_Load load(Directories::get_save_directory()+"game_commands.cfg");

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

            //Load data based on the line.

            else if(!multi_line_comment && !boost::algorithm::starts_with(line,"</game_commands>")){
                vector<string> command_parts;
                boost::algorithm::split(command_parts,line,boost::algorithm::is_any_of(","));

                if(command_parts.size()==7){
                    Game_Command* game_command=0;

                    string name=command_parts[0];

                    for(int i=0;i<game_commands.size();i++){
                        if(name==game_commands[i].name){
                            game_command=&game_commands[i];

                            break;
                        }

                        if(i==game_commands.size()-1){
                            game_commands.push_back(Game_Command());
                            game_command=&game_commands[game_commands.size()-1];

                            break;
                        }
                    }

                    if(game_command!=0){
                        SDL_Scancode key=SDL_SCANCODE_UNKNOWN;
                        if(command_parts[4].length()>0){
                            key=SDL_GetScancodeFromName(command_parts[4].c_str());
                        }

                        SDL_GameControllerButton button=SDL_CONTROLLER_BUTTON_INVALID;
                        if(command_parts[5].length()>0){
                            button=SDL_GameControllerGetButtonFromString(command_parts[5].c_str());
                        }

                        SDL_GameControllerAxis axis=SDL_CONTROLLER_AXIS_INVALID;
                        if(command_parts[6].length()>0){
                            axis=SDL_GameControllerGetAxisFromString(command_parts[6].c_str());
                        }

                        game_command->name=name;
                        game_command->title=command_parts[1];
                        game_command->description=Strings::process_newlines(command_parts[2]);
                        game_command->dev=Strings::string_to_bool(command_parts[3]);
                        game_command->key=key;
                        game_command->button=button;
                        game_command->axis=axis;
                    }
                }
            }

            //If the line ends the game commands data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</game_commands>")){
                break;
            }
        }
    }

    if(!save_game_commands()){
        return false;
    }

    return true;
}

bool Engine_Interface::save_servers(){
    stringstream save("");

    save<<"<servers>\n";

    for(int i=0;i<network.server_list.size();i++){
        save<<"\t"<<network.server_list[i].name<<","<<network.server_list[i].address<<","<<network.server_list[i].port<<","<<network.server_list[i].password<<"\n";
    }

    save<<"</servers>\n";

    if(File_IO::save_atomic(Directories::get_save_directory()+"servers.txt",save.str(),true)){
        return true;
    }
    else{
        return false;
    }
}

bool Engine_Interface::load_servers(){
    File_IO_Load load(Directories::get_save_directory()+"servers.txt");

    if(load.is_opened()){
        bool multi_line_comment=false;

        network.server_list.clear();

        while(!load.eof()){
            string line="";

            load.getline(&line);

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

            else if(!multi_line_comment && !boost::algorithm::starts_with(line,"</servers>")){
                vector<string> command_parts;
                boost::algorithm::split(command_parts,line,boost::algorithm::is_any_of(","));

                if(command_parts.size()==4){
                    network.server_list.push_back(Server());
                    network.server_list[network.server_list.size()-1].name=command_parts[0];
                    network.server_list[network.server_list.size()-1].address=command_parts[1];
                    network.server_list[network.server_list.size()-1].port=Strings::string_to_unsigned_long(command_parts[2]);
                    network.server_list[network.server_list.size()-1].password=command_parts[3];
                }
            }

            //If the line ends the data.
            else if(!multi_line_comment && boost::algorithm::starts_with(line,"</servers>")){
                break;
            }
        }
    }
    else{
        return false;
    }

    return true;
}
