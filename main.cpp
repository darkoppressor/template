/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "main.h"
#include "game_data.h"

#include <main_startup.h>

#ifdef GAME_OS_ANDROID
    #include <SDL.h>
    #include <jni.h>
#endif

using namespace std;

#ifdef GAME_OS_ANDROID
    //This block of code was taken from SDL_android_main.c
    //I was having trouble compiling that file along with the game code to create the library for Android
    //Simply copying that code here seems to fix things...
    extern "C"{
        /* Called before SDL_main() to initialize JNI bindings in SDL library */
        extern void SDL_Android_Init(JNIEnv* env, jclass cls);

        /* Start up the SDL app */
        void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject obj)
        {
            /* This interface could expand with ABI negotiation, calbacks, etc. */
            SDL_Android_Init(env, cls);

            SDL_SetMainReady();

            /* Run the application code! */
            int status;
            char *argv[2];
            argv[0] = SDL_strdup("SDL_app");
            argv[1] = NULL;
            status = SDL_main(1, argv);

            /* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
            /* exit(status); */
        }
    }
#endif

int main(int argc,char* args[]){
    return main_startup(Game_Data::game_data_load_item_count);
}
