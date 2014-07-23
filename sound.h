#ifndef sound_h
#define sound_h

#include "sound_data.h"

#include <vector>
#include <string>

#include <SDL_mixer.h>

class Sound{
public:

    std::vector<Sound_Data> sounds;

    std::vector<std::string> sound_names;

    void load_sounds();
    void unload_sounds();

    //Modifies volume by applying the appropriate audio option multipliers.
    int modify_sound_volume(int volume);

    int get_free_channel();

    //If coordinates are not given, the sound is played at maximum volume.
    //falloff determines the rate at which a positional sound falls off.
    //The higher falloff is, the more slowly sounds will fade out as the distance between the sound receiver and the sound source increases.
    //So a low falloff means the sound will quickly falloff.
    void play_sound(std::string sound_name,double sound_x=-1.0,double sound_y=-1.0,double falloff=-1.0);

    Sound_Data* get_sound(std::string sound_name);
};

#endif
