/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "game_mailman.h"

#include <engine_mailman.h>

using namespace std;

bool Game_Mailman::has_mail(){
    return !Engine_Mailman::mail_out.empty();
}

string Game_Mailman::get_letter(){
    if(has_mail()){
        string letter=Engine_Mailman::mail_out.front();

        Engine_Mailman::mail_out.pop();

        return letter;
    }
    else{
        return "";
    }
}
