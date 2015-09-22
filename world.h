/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef world_h
#define world_h

#include "game_window.h"
#include "engine_interface.h"
#include "update.h"
#include "game.h"
#include "game_constants.h"
#include "network.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdio>

extern Game_Window main_window;

extern Engine_Interface engine_interface;

extern Update update;

extern Network network;

extern Game game;

bool load_world();

void unload_world();

#endif
