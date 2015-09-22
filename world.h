/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef world_h
#define world_h

#include "game_window.h"
#include "engine_interface.h"
#include "update.h"
#include "android.h"
#include "game.h"
#include "game_constants.h"
#include "network.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdio>

const int CONTROLLER_ID_ALL=-1;
const int CONTROLLER_ID_TOUCH=-2;

extern std::string CHECKSUM;

extern double UPDATE_RATE;
extern double SKIP_TICKS;

extern double UPDATE_RATE_RENDER;
extern double SKIP_TICKS_RENDER;

extern Game_Window main_window;

extern SDL_Event event;

extern Engine_Interface engine_interface;

extern Update update;

extern Android android;

extern Network network;

extern Game game;

bool load_world();

void unload_world();

#endif
