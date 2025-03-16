/* Copyright (c) Cheese and Bacon Games, LLC. See docs/LICENSE.md for details. */

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
