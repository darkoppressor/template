/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef version_h
#define version_h

#include <string>

class Version{
public:

    static std::string get_year();
    static std::string get_month();
    static std::string get_day();

    static int get_major();
    static int get_minor();
    static int get_micro();

    static std::string get_status();
};

#endif
