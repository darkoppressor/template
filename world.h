/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef world_h
#define world_h

#include "engine_interface.h"
#include "network.h"

extern Engine_Interface engine_interface;

extern Network network;

bool load_world();

void unload_world();

#endif
