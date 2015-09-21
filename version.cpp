/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "version.h"

using namespace std;

string Version::get_year(){
    return "2015"; //YEAR
}

string Version::get_month(){
    return "09"; //MONTH
}

string Version::get_day(){
    return "20"; //DAY
}

int Version::get_major(){
    return 0; //MAJOR
}

int Version::get_minor(){
    return 0; //MINOR
}

int Version::get_micro(){
    return 1; //MICRO
}

string Version::get_status(){
    return "alpha"; //STATUS
}
