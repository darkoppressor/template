/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "sprite.h"
#include "image_manager.h"
#include "render.h"
#include "object_manager.h"

#include <cmath>

using namespace std;

Sprite::Sprite(){
    name="";

    reset_animation();

    animating=true;
}

double Sprite::get_width(){
    if(is_animated()){
        return Object_Manager::get_animation(name)->sprite_sheet[frame].w;
    }
    else{
        return Image_Manager::get_image(name)->w;
    }
}

double Sprite::get_height(){
    if(is_animated()){
        return Object_Manager::get_animation(name)->sprite_sheet[frame].h;
    }
    else{
        return Image_Manager::get_image(name)->h;
    }
}

bool Sprite::is_animated(){
    if(Object_Manager::get_animation(name)!=0){
        return true;
    }
    else{
        return false;
    }
}

void Sprite::reset_animation(){
    frame=0;
    frame_counter=0;
}

void Sprite::set_name(string get_name){
    name=get_name;

    reset_animation();
}

void Sprite::animate(int animation_speed_override){
    if(is_animated() && animating){
        int animation_speed=Object_Manager::get_animation(name)->animation_speed;
        if(animation_speed_override!=-1){
            animation_speed=animation_speed_override;
        }

        if(animation_speed!=-1 && ++frame_counter>=(int)ceil(((double)animation_speed/1000.0)*UPDATE_RATE)){
            frame_counter=0;

            if(++frame>Object_Manager::get_animation(name)->frame_count-1){
                frame=0;

                if(Object_Manager::get_animation(name)->end_behavior=="stop"){
                    animating=false;
                }
            }
        }
    }
}

void Sprite::render(SDL_Renderer* renderer,double x,double y,double opacity,double scale_x,double scale_y,double angle,string color_name){
    if(is_animated()){
        Render::render_sprite(renderer,x,y,Image_Manager::get_image(name),&Object_Manager::get_animation(name)->sprite_sheet[frame],opacity,scale_x,scale_y,angle,color_name);
    }
    else{
        Render::render_texture(renderer,x,y,Image_Manager::get_image(name),opacity,scale_x,scale_y,angle,color_name);
    }
}
