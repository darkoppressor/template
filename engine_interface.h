/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef engine_interface_h
#define engine_interface_h

#include "special_info.h"
#include "button_events.h"
#include "font.h"
#include "cursor.h"
#include "tooltip.h"
#include "toast.h"
#include "window.h"
#include "console.h"
#include "game_command.h"
#include "game_constants.h"

///#include "example_game_tag.h"

#include <coords.h>
#include <file_io.h>
#include <rng.h>
#include <color_theme.h>
#include <custom_sound.h>
#include <sprite.h>
#include <game_option.h>

#include <string>

class Version_Series{
public:

    int major_1;
    int minor_1;
    int micro_1;

    int major_2;
    int minor_2;
    int micro_2;

    Version_Series(int get_major_1,int get_minor_1,int get_micro_1,int get_major_2,int get_minor_2,int get_micro_2);
};

class GUI_Object{
public:

    //Valid values:
    //button
    //information
    std::string type;

    int index;

    int x;
    int y;

    bool sort_by_y;

    GUI_Object(std::string get_type,int get_index,int get_x,int get_y);

    bool operator<=(GUI_Object object);

    static void set_sort_by_y(std::vector<GUI_Object>& objects,bool sort_value);
};

class GUI_Selector_Chaser{
public:

    double x;
    double y;

    GUI_Selector_Chaser();
};

class Engine_Interface{
public:

    Special_Info special_info_manager;
    Button_Events button_events_manager;

    std::vector<Bitmap_Font> fonts;
    std::vector<Cursor> cursors;
    std::vector<Color_Theme> color_themes;
    std::vector<Window> windows;

    ///std::vector<Example_Game_Tag> example_game_tags;

    Tooltip tooltip;

    std::vector<Toast> toasts;

    Console console;

    Console chat;

    //The z order of all open windows.
    std::vector<Window*> window_z_order;
    //The window that the mouse is currently over.
    Window* window_under_mouse;

    bool hide_gui;

    RNG rng;

    //Set this to true during runtime to tell the engine to reinitialize everything.
    bool need_to_reinit;

    //If true, some button is moused over this frame.
    bool mouse_over;

    //Possible values:
    //mouse
    //keyboard
    //controller
    std::string gui_mode;

    //-1 if no object is selected.
    int gui_selected_object;

    std::string gui_selector_style;

    std::vector<GUI_Selector_Chaser> gui_selector_chasers;

    std::vector<Game_Command> game_commands;

    std::vector<Game_Option> game_options;

    std::string gui_axis_nav_last_direction;

    int counter_gui_scroll_axis;

    int configure_command;

    std::vector<std::string> characters_lower;
    std::vector<std::string> characters_upper;
    std::vector<std::string> characters_numbers;
    std::vector<std::string> characters_symbols;

    Coords text_entry_small_selector;

    Sprite text_selector;

    int editing_server;

    //Mutable text cursor.
    //The time to spend on each step of fading animation, in milliseconds
    int counter_cursor;
    bool cursor_fade_direction;
    int cursor_opacity;

    //Pointer to the current mutable text Information object, if any
    Information* ptr_mutable_info;

    Engine_Interface();

    //Reinitialize the game window when next possible
    void reload();

    void quit();

    void build_text_input_characters();

    void set_logic_update_rate(double frame_rate);
    void set_render_update_rate(double frame_rate);

    //Loads only engine data.
    //Returns false if engine data could not be loaded.
    //Returns true otherwise.
    bool load_data_engine();

    void load_data_game();

    //Loads everything but engine data and game options.
    void load_data_main();

    //Loads only game options.
    void load_data_game_options();

    //Load any data of the passed tag type.
    //Returns false if passed tag type's data could not be loaded.
    //Returns true otherwise.
    void load_data_tag_game(std::string tag,File_IO_Load* load);

    //Load any data of the passed tag type.
    //Returns false if passed tag type's data could not be loaded.
    //Returns true otherwise.
    bool load_data(std::string tag);

    void unload_data_game();
    void unload_data();

    void load_engine_data(File_IO_Load* load);
    void load_font(File_IO_Load* load);
    void load_cursor(File_IO_Load* load);
    void load_color_theme(File_IO_Load* load);
    void load_window(File_IO_Load* load);
    void load_information(File_IO_Load* load);
    void load_button(File_IO_Load* load);
    void load_game_command(File_IO_Load* load);
    void load_game_option(File_IO_Load* load);
    void load_game_constant(File_IO_Load* load);
    void load_custom_sound(File_IO_Load* load);
    void load_custom_sound_data(File_IO_Load* load,Custom_Sound& sound);

    Bitmap_Font* get_font(std::string name);
    Cursor* get_cursor(std::string name);
    Color_Theme* get_color_theme(std::string name);
    Window* get_window(std::string name);
    Game_Command* get_game_command(std::string name);
    Game_Option* get_game_option(std::string name);

    ///void load_example_game_tag(File_IO_Load* load);
    ///Example_Game_Tag* get_example_game_tag(std::string name);

    Color_Theme* current_color_theme();

    //Rebuild any window data if needed.
    void rebuild_window_data();

    bool is_window_open(Window* window);
    bool is_window_on_top(Window* window);
    Window* get_top_window();
    void open_window(Window* window);
    void close_window(Window* window);
    void bring_window_to_top(Window* window);

    bool is_exclusive_window_open();
    bool is_any_window_open();
    int open_window_count();
    void close_all_windows(Window* ignore=0);
    void close_top_window();
    bool is_mouse_over_window();
    bool is_mouse_over_open_window();

    bool poll_event(SDL_Event* event_storage);

    bool mouse_allowed();

    //Check if we need to scroll the scrolling button list.
    void gui_check_scrolling_button();
    void reset_gui_selector_chasers();
    void reset_gui_selected_object();
    void set_gui_mode(std::string new_gui_mode);
    //Returns the number of GUI objects on the currently focused window.
    //Returns 0 if no window is currently in focus.
    int get_gui_object_count();
    void change_gui_selected_object(std::string direction);
    //Returns a GUI_Object for the selected GUI object.
    //If there is no selected GUI object, the GUI_Object's type will equal none.
    GUI_Object get_gui_selected_object();
    //Returns a Engine_Rect with the position and size of the selected GUI object.
    //If there is no selected GUI object, returns a Engine_Rect of all -1's.
    Engine_Rect get_gui_selected_object_pos();
    bool is_gui_object_selected(Button* button_to_check);
    bool is_gui_object_selected(Information* info_to_check);
    //Activates the currently selected GUI object.
    void confirm_gui_object();
    void scroll_gui_object_up();
    void scroll_gui_object_down();

    void get_mouse_state(int* mouse_x,int* mouse_y);

    void update_window_caption(int render_rate,double ms_per_frame,int logic_frame_rate);

    //Sets the passed info to be the currently selected mutable.
    void set_mutable_info(Information* ptr_info);
    //Clear the selected info mutable.
    void clear_mutable_info();
    //Returns true if there is a mutable info selected,
    //false otherwise.
    bool mutable_info_selected();
    //Returns true if the passed info matches the selected mutable info.
    bool mutable_info_this(Information* ptr_info);

    void add_chat(std::string message);

    void make_notice(std::string message);

    void make_toast(std::string message,std::string length="medium",int custom_length=-1);

    void handle_text_input(std::string text);

    //If the game command uses a key or button:
    //Returns 1 if the passed game command's corresponding input is currently pressed.
    //Returns 0 if the passed game command's corresponding input is currently unpressed.
    //If the game command uses an axis:
    //Returns the axis state as a value from -32768 to 32767.
    int game_command_state(std::string name);

    //Only used by Android hardware buttons (I think...):
    void gui_nav_back_android();
    void gui_nav_toggle_menu_android();

    //Only used by controller mode:
    void gui_nav_back_controller();
    void gui_nav_toggle_menu_controller();

    //Only used by mouse and keyboard modes:
    void gui_nav_back_keyboard_and_mouse();

    //Used by all control modes:
    void gui_nav_confirm(std::string gui_type);
    void gui_nav_up(std::string gui_type);
    void gui_nav_down(std::string gui_type);
    void gui_nav_left();
    void gui_nav_right();
    void gui_scroll_up(std::string gui_type);
    void gui_scroll_down(std::string gui_type);

    //Returns true if there is a mutable info selected, and it is the console.
    bool is_console_selected();

    //Returns true if there is a mutable info selected, and it is the chat box.
    bool is_chat_selected();

    int get_text_input_selected_chunk();
    std::vector<std::string>* get_text_input_character_set();

    void input_backspace();
    void input_delete();
    void input_newline();

    bool allow_screen_keyboard();

    std::string get_text_entry_small_character();

    void prepare_for_input();

    bool handle_input_events_drag_and_drop();
    bool handle_input_events_touch();
    bool handle_input_events_command_set();
    bool handle_input_events(bool event_ignore_command_set);
    void handle_input_states();
    void animate();
    void animate_gui_selector_chasers();
    void render_title_background();
    void render_pause();
    void render_fps(int render_rate,double ms_per_frame,int logic_frame_rate);
    void render_gui_selector();
    void render_small_text_inputter();
    void render_text_inputter();
    void render_text_editing();
    void render(int render_rate,double ms_per_frame,int logic_frame_rate);
    void render_loading_screen(double percentage,std::string load_message);

    void handle_drag_and_drop(std::string file);

    std::string get_game_window_caption();

    void render_dev_info();
    std::string get_system_info();

    //** Options **//

    std::string get_version();
    std::string get_build_date();

    bool save_save_location();
    bool load_save_location();

    //Compares two versions.
    //Returns 0 if they are the same.
    //Returns -1 if version 1 is less than version 2.
    //Returns 1 if version 1 is greater than version 2.
    int version_compare(int major_1,int minor_1,int micro_1,int major_2,int minor_2,int micro_2);

    //Returns the index into the passed vector that the passed version is a part of.
    //Returns -1 if the passed version is not in any series.
    int which_version_series(std::vector<Version_Series>* version_series,int major,int minor,int micro);

    //Returns true if the version of the passed profile is in the same series of compatible releases as the current version.
    bool version_compatible(std::string name_to_check);

    std::string get_option_value(std::string option);
    std::string get_option_description(std::string option);
    void change_option(std::string option,std::string new_value);

    void apply_options(const std::string& cl_effect_limit,const std::string& cl_screen_shake,const std::string& cl_rumble);

    void apply_options_graphics(const std::string& cl_screen_width,const std::string& cl_screen_height,const std::string& cl_display_number,
                                const std::string& cl_fullscreen_state,const std::string& cl_fullscreen_mode,const std::string& cl_vsync,
                                const std::string& cl_fps,const std::string& cl_hw_cursor,const std::string& cl_font_shadows);

    void apply_options_audio(const std::string& cl_volume_global,const std::string& cl_mute_global,
                             const std::string& cl_volume_sound,const std::string& cl_mute_sound,
                             const std::string& cl_volume_music,const std::string& cl_mute_music);

    void apply_options_input(const std::string& cl_bind_cursor,const std::string& cl_screen_keyboard,const std::string& cl_accelerometer_controller,
                             const std::string& cl_touch_controller_state,const std::string& cl_touch_controller_opacity);

    void apply_options_network(const std::string& cl_name,const std::string& cl_chat_timestamps);

    //Returns false if options could not be saved.
    //Returns true otherwise.
    bool save_options();
    //Returns false if options could not be loaded.
    //Returns true otherwise.
    bool load_options();

    //Returns false if game commands could not be saved.
    //Returns true otherwise.
    bool save_game_commands();
    //Returns false if game commands could not be loaded.
    //Returns true otherwise.
    bool load_game_commands();

    //Returns false if servers could not be saved.
    //Returns true otherwise.
    bool save_servers();
    //Returns false if servers could not be loaded.
    //Returns true otherwise.
    bool load_servers();
};

#endif
