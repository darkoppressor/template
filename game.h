/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef game_h
#define game_h

// #include "example_object.h"

#include <vector>

class Game {
    public:
        // static std::vector<Example_Object> example_objects;

        static void clear_world();
        static void generate_world();
        static void tick();
        static void ai();
        static void movement();
        static void events();
        static void animate();
        static void render();
        static void render_to_textures();
        static void update_background();
        static void render_background();
};

#endif
