/* Copyright (c) Cheese and Bacon Games, LLC. See docs/LICENSE.md for details. */

#include <engine.h>
#include <game/game_manager.h>
#include <ui/font.h>
#include <data/object_manager.h>
#include <options/options.h>
#include <network/network_engine.h>
#include <render/render.h>
#include <strings/engine_strings.h>

using namespace std;

void Engine::render_dev_info () {
    string msg = "";

    if (Game_Manager::in_progress) {
        msg += "Camera Position: " + Strings::num_to_string(Game_Manager::camera.x) + "," +
               Strings::num_to_string(Game_Manager::camera.y) + "\n";
        msg += "Camera Size: " + Strings::num_to_string(Game_Manager::camera.w / Game_Manager::camera_zoom) + "," +
               Strings::num_to_string(Game_Manager::camera.h / Game_Manager::camera_zoom) + "\n";
        msg += "Camera Zoom: " + Strings::num_to_string(Game_Manager::camera_zoom) + "\n";
    }

    if (msg.length() > 0) {
        Bitmap_Font* font = Object_Manager::get_font("small");
        double y = 2.0;

        if (Options::fps) {
            y += font->spacing_y;

            if (Network_Engine::status != "off") {
                y += font->spacing_y * 2.0;
            }
        }

        Render::render_rectangle(2.0, y, Strings::longest_line(msg) * font->spacing_x,
                                 Strings::newline_count(msg) * font->spacing_y, 0.75, "ui_black");
        font->show(2.0, y, msg, "red");
    }
}
