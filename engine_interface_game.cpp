/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "engine_interface.h"
#include "world.h"

#include <strings.h>
#include <log.h>
#include <data_reader.h>
#include <image_data.h>
#include <image_manager.h>
#include <render.h>
#include <engine.h>
#include <font.h>
#include <object_manager.h>
#include <game_window.h>

using namespace std;

void Engine_Interface::load_data_game(){
    ///load_data("example_game_tag");
}

void Engine_Interface::load_data_tag_game(string tag,File_IO_Load* load){
    /**if(tag=="example_game_tag"){
        load_example_game_tag(load);
    }*/
}

void Engine_Interface::unload_data_game(){
    ///example_game_tags.clear();
}

/**void Engine_Interface::load_example_game_tag(File_IO_Load* load){
    example_game_tags.push_back(Example_Game_Tag());

    vector<string> lines=Data_Reader::read_data(load,"</example_game_tag>");

    for(size_t i=0;i<lines.size();i++){
        string& line=lines[i];

        if(Data_Reader::check_prefix(line,"name:")){
            example_game_tags.back().name=line;
        }
    }
}*/

/**Example_Game_Tag* Engine_Interface::get_example_game_tag(string name){
    Example_Game_Tag* ptr_object=0;

    for(size_t i=0;i<example_game_tags.size();i++){
        if(example_game_tags[i].name==name){
            ptr_object=&example_game_tags[i];

            break;
        }
    }

    if(ptr_object==0){
        Log::add_error("Error accessing example game tag '"+name+"'");
    }

    return ptr_object;
}*/

void Engine_Interface::handle_drag_and_drop(string file){
    ///Do something with file
}

void Engine_Interface::render_title_background(){
    Bitmap_Font* font=Object_Manager::get_font("small");

    Render::render_rectangle(0,0,Game_Window::SCREEN_WIDTH,Game_Window::SCREEN_HEIGHT,1.0,"ui_black");

    font->show(0,Game_Window::SCREEN_HEIGHT-font->spacing_y*2.0,"Version "+get_version()+"\nChecksum "+Engine::CHECKSUM,"ui_white");

    Image_Data* logo=Image_Manager::get_image("logo");

    double logo_scale_x=(double)Game_Window::SCREEN_WIDTH/(double)1280.0;
    double logo_scale_y=(double)Game_Window::SCREEN_HEIGHT/(double)720.0;

    Render::render_texture(Game_Window::SCREEN_WIDTH-logo->w*logo_scale_x,Game_Window::SCREEN_HEIGHT-logo->h*logo_scale_y,logo,1.0,logo_scale_x,logo_scale_y);
}

void Engine_Interface::render_pause(){
    Bitmap_Font* font=Object_Manager::get_font("standard");

    string msg="Paused";
    font->show((Game_Window::SCREEN_WIDTH-(font->spacing_x*msg.length()))/2,(Game_Window::SCREEN_HEIGHT-font->spacing_y)/2,msg,"ui_white");
}

void Engine_Interface::render_fps(int render_rate,double ms_per_frame,int logic_frame_rate){
    Object_Manager::get_font("small")->show(2,2,"FPS: "+Strings::num_to_string(render_rate)+"\n"+network.get_stats(),"ui_white");
}

void Engine_Interface::render_loading_screen(double percentage,string load_message){
    Bitmap_Font* font=Object_Manager::get_font("standard");

    Game_Window::clear_renderer(Color(0,0,0,255));

    ///If images are loaded
    ///Render::render_texture(0,0,Image_Manager::get_image("loading_screen"),0.25);

    Render::render_rectangle(0,0,Game_Window::SCREEN_WIDTH,Game_Window::SCREEN_HEIGHT,1.0,"ui_black");

    double bar_width=240.0*percentage;
    double max_bar_width=240.0*1.0;
    Render::render_rectangle(Game_Window::SCREEN_WIDTH/2.0-120-2,Game_Window::SCREEN_HEIGHT-75-2,max_bar_width+4,30+4,1.0,"ui_3");
    Render::render_rectangle(Game_Window::SCREEN_WIDTH/2.0-120,Game_Window::SCREEN_HEIGHT-75,bar_width,30,1.0,"ui_1");

    string msg=Strings::num_to_string((int)(percentage*100.0))+"%";

    font->show(Game_Window::SCREEN_WIDTH/2.0-120+(max_bar_width-msg.length()*font->spacing_x)/2.0,Game_Window::SCREEN_HEIGHT-75+font->spacing_y/4.0,msg,"ui_0");

    font->show((Game_Window::SCREEN_WIDTH-load_message.length()*font->spacing_x)/2.0,Game_Window::SCREEN_HEIGHT-75+font->spacing_y*2.0,load_message,"ui_0");

    Game_Window::render_present();
}

string Engine_Interface::get_game_window_caption(){
    string msg="";

    return msg;
}
