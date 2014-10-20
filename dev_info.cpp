#include "engine_interface.h"
#include "world.h"
#include "render.h"

using namespace std;

void Engine_Interface::render_dev_info(){
    string msg="";

    if(game.in_progress){
        msg+="Camera Position: "+Strings::num_to_string(game.camera.x*game.camera_zoom)+","+Strings::num_to_string(game.camera.y*game.camera_zoom)+"\n";
        msg+="Camera Size: "+Strings::num_to_string(game.camera.w/game.camera_zoom)+","+Strings::num_to_string(game.camera.h/game.camera_zoom)+"\n";
        msg+="Camera Zoom: "+Strings::num_to_string(game.camera_zoom)+"\n";
    }

    if(msg.length()>0){
        Bitmap_Font* font=engine_interface.get_font("small");

        render_rectangle(2.0,2.0,Strings::longest_line(msg)*font->spacing_x,Strings::newline_count(msg)*font->spacing_y,0.75,"ui_black");
        font->show(2.0,2.0,msg,"red");
    }
}
