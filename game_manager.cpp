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
