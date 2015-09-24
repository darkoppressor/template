/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "world.h"

#include <game_manager.h>
#include <options.h>
#include <music_manager.h>
#include <screen_shake.h>
#include <engine.h>
#include <render.h>
#include <font.h>
#include <object_manager.h>

using namespace std;

bool Game_Manager::effect_allowed(){
    uint32_t effects=/**Game_Objects::effects_example.size()*/0;

    if(effects<Options::effect_limit){
        return true;
    }
    else{
        return false;
    }
}

void Game_Manager::manage_music(){
    string music_to_play="";

    /**if(in_progress){
        music_to_play="1";
    }
    else{
        music_to_play="";
    }*/

    if(music_to_play.length()>0){
        if(music_to_play!=current_music){
            Music_Manager::restart_track(music_to_play);
        }

        Music_Manager::play_track(music_to_play);
    }
    else{
        Music_Manager::stop_track();
    }

    current_music=music_to_play;
}

void Game_Manager::stop(){
    network.stop();

    if(in_progress){
        in_progress=false;

        reset();

        Game_World::clear_world();

        if(engine_interface.chat.on){
            engine_interface.chat.toggle_on();
        }
    }
}

void Game_Manager::set_camera(){
    camera_delta_x=camera.x;
    camera_delta_y=camera.y;

    Screen_Shake::update_camera_before(camera);

    if(false/**Something to follow*/){
    }
    else{
        if(cam_state=="left"){
            camera.x-=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="up"){
            camera.y-=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="right"){
            camera.x+=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="down"){
            camera.y+=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="left_up"){
            camera.x-=camera_speed/Engine::UPDATE_RATE;
            camera.y-=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="right_up"){
            camera.x+=camera_speed/Engine::UPDATE_RATE;
            camera.y-=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="right_down"){
            camera.x+=camera_speed/Engine::UPDATE_RATE;
            camera.y+=camera_speed/Engine::UPDATE_RATE;
        }
        else if(cam_state=="left_down"){
            camera.x-=camera_speed/Engine::UPDATE_RATE;
            camera.y+=camera_speed/Engine::UPDATE_RATE;
        }
    }

    //If the camera goes past the bounds of the world, set it back in the bounds.
    if(camera.x<0.0){
        camera.x=0.0;
    }
    if(camera.x+camera.w>0/**World size x*/*camera_zoom){
        camera.x=0/**World size x*/*camera_zoom-camera.w;
    }
    if(camera.y<0.0){
        camera.y=0.0;
    }
    if(camera.y+camera.h>0/**World size y*/*camera_zoom){
        camera.y=0/**World size y*/*camera_zoom-camera.h;
    }

    Screen_Shake::update_camera_after(camera);

    camera_delta_x=camera.x-camera_delta_x;
    camera_delta_y=camera.y-camera_delta_y;
}

void Game_Manager::handle_drag_and_drop(string file){
    ///Do something with file
}

string Game_Manager::get_game_window_caption(){
    string msg="";

    return msg;
}

void Game_Manager::render_scoreboard(){
    if(display_scoreboard){
        Bitmap_Font* font=Object_Manager::get_font("small");

        string name_list=network.get_name_list();
        string ping_list=network.get_ping_list();

        Render::render_rectangle(0,0,Game_Window::SCREEN_WIDTH,Game_Window::SCREEN_HEIGHT,0.5,"ui_black");

        font->show(72,(Game_Window::SCREEN_HEIGHT-(Strings::newline_count(name_list)+1)*font->spacing_y)/2.0,name_list,"ui_white");
        font->show(168,(Game_Window::SCREEN_HEIGHT-(Strings::newline_count(ping_list)+1)*font->spacing_y)/2.0,ping_list,"ui_white");
    }
}

void Game_Manager::render_title_background(){
    Bitmap_Font* font=Object_Manager::get_font("small");

    Render::render_rectangle(0,0,Game_Window::SCREEN_WIDTH,Game_Window::SCREEN_HEIGHT,1.0,"ui_black");

    font->show(0,Game_Window::SCREEN_HEIGHT-font->spacing_y*2.0,"Version "+get_version()+"\nChecksum "+Engine::CHECKSUM,"ui_white");

    Image_Data* logo=Image_Manager::get_image("logo");

    double logo_scale_x=(double)Game_Window::SCREEN_WIDTH/(double)1280.0;
    double logo_scale_y=(double)Game_Window::SCREEN_HEIGHT/(double)720.0;

    Render::render_texture(Game_Window::SCREEN_WIDTH-logo->w*logo_scale_x,Game_Window::SCREEN_HEIGHT-logo->h*logo_scale_y,logo,1.0,logo_scale_x,logo_scale_y);
}

void Game_Manager::render_pause(){
    Bitmap_Font* font=Object_Manager::get_font("standard");

    string msg="Paused";
    font->show((Game_Window::SCREEN_WIDTH-(font->spacing_x*msg.length()))/2,(Game_Window::SCREEN_HEIGHT-font->spacing_y)/2,msg,"ui_white");
}

void Game_Manager::render_fps(int render_rate,double ms_per_frame,int logic_frame_rate){
    Object_Manager::get_font("small")->show(2,2,"FPS: "+Strings::num_to_string(render_rate)+"\n"+network.get_stats(),"ui_white");
}

void Game_Manager::render_loading_screen(double percentage,string load_message){
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

void Game_Manager::load_data_game(){
    Game_Data::load_data_game();
}

void Game_Manager::load_data_tag_game(string tag,File_IO_Load* load){
    Game_Data::load_data_tag_game(tag,load);
}

void Game_Manager::unload_data_game(){
    Game_Data::unload_data_game();
}
