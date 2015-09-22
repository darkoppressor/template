/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "toast.h"
#include "world.h"

#include <strings.h>
#include <render.h>
#include <engine_data.h>

using namespace std;

Toast::Toast(string get_message,double get_fade_rate){
    message=get_message;
    fade_rate=get_fade_rate;

    opacity=1.0;

    string font=Engine_Data::toast_font;

    set_dimensions(font);

    x=(main_window.SCREEN_WIDTH-w)/2.0;
    y=main_window.SCREEN_HEIGHT-h-engine_interface.get_font(font)->spacing_y;
}

void Toast::set_dimensions(string font){
    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    w=Engine_Data::gui_border_thickness*2.0+Strings::longest_line(message)*ptr_font->spacing_x+ptr_font->gui_padding_x;
    h=Engine_Data::gui_border_thickness*2.0+(Strings::newline_count(message)+1)*ptr_font->spacing_y+ptr_font->gui_padding_y;
}

bool Toast::is_done(){
    if(opacity==0.0){
        return true;
    }
    else{
        return false;
    }
}

void Toast::animate(){
    opacity-=fade_rate;

    if(opacity<0.0){
        opacity=0.0;
    }
}

void Toast::render(){
    string font=Engine_Data::toast_font;

    Bitmap_Font* ptr_font=engine_interface.get_font(font);

    //Render the border.
    if(engine_interface.current_color_theme()->toast_border!="<INVISIBLE>"){
        Render::render_rectangle(main_window.renderer,x,y,w,h,opacity,engine_interface.current_color_theme()->toast_border);
    }

    //Render the background.
    if(engine_interface.current_color_theme()->toast_background!="<INVISIBLE>"){
        Render::render_rectangle(main_window.renderer,x+Engine_Data::gui_border_thickness,y+Engine_Data::gui_border_thickness,w-Engine_Data::gui_border_thickness*2.0,h-Engine_Data::gui_border_thickness*2.0,opacity,engine_interface.current_color_theme()->toast_background);
    }

    //Display the message text.
    if(engine_interface.current_color_theme()->toast_font!="<INVISIBLE>"){
        ptr_font->show(x+Engine_Data::gui_border_thickness,y+Engine_Data::gui_border_thickness,message,engine_interface.current_color_theme()->toast_font,opacity);
    }
}
