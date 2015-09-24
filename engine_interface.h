/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef engine_interface_h
#define engine_interface_h

#include "console.h"

#include <coords.h>
#include <file_io.h>
#include <rng.h>
#include <sprite.h>

#include <string>

class Engine_Interface{
public:

    Console console;

    Console chat;

    RNG rng;

    bool poll_event(SDL_Event* event_storage);

    void update_window_caption(int render_rate,double ms_per_frame,int logic_frame_rate);

    void add_chat(std::string message);

    //Returns true if there is a mutable info selected, and it is the console
    bool is_console_selected();

    //Returns true if there is a mutable info selected, and it is the chat box
    bool is_chat_selected();

    void input_backspace();
    void input_delete();
    void input_newline();

    void prepare_for_input();

    bool handle_input_events_drag_and_drop();
    bool handle_input_events_touch();
    bool handle_input_events(bool event_ignore_command_set);
    void handle_input_states();
    void animate();
    void render(int render_rate,double ms_per_frame,int logic_frame_rate);

    void render_dev_info();
    std::string get_system_info();
};

#endif
