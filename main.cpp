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
        JNIEXPORT int JNICALL Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject array)
        {
            int i;
            int argc;
            int status;

            /* This interface could expand with ABI negotiation, callbacks, etc. */
            SDL_Android_Init(env, cls);

            SDL_SetMainReady();

            /* Prepare the arguments. */

            int len = (*env)->GetArrayLength(env, array);
            char* argv[1 + len + 1];
            argc = 0;
            /* Use the name "app_process" so PHYSFS_platformCalcBaseDir() works.
               https://bitbucket.org/MartinFelis/love-android-sdl2/issue/23/release-build-crash-on-start
             */
            argv[argc++] = SDL_strdup("app_process");
            for (i = 0; i < len; ++i) {
                const char* utf;
                char* arg = NULL;
                jstring string = (*env)->GetObjectArrayElement(env, array, i);
                if (string) {
                    utf = (*env)->GetStringUTFChars(env, string, 0);
                    if (utf) {
                        arg = SDL_strdup(utf);
                        (*env)->ReleaseStringUTFChars(env, string, utf);
                    }
                    (*env)->DeleteLocalRef(env, string);
                }
                if (!arg) {
                    arg = SDL_strdup("");
                }
                argv[argc++] = arg;
            }
            argv[argc] = NULL;


            /* Run the application. */

            status = SDL_main(argc, argv);

            /* Release the arguments. */

            for (i = 0; i < argc; ++i) {
                SDL_free(argv[i]);
            }

            /* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
            /* exit(status); */

            return status;
        }
    }
#endif

int main(int argc,char* args[]){
    return main_startup(Game_Data::game_data_load_item_count);
}
