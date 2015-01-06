#ifndef custom_sound_h
#define custom_sound_h

#include "file_io.h"

#include <string>
#include <vector>
#include <stdint.h>

class Custom_Sound{
private:

    template<typename T>
    void write_binary(File_IO_Binary_Save& save,const T& t){
        save.write((const char*)&t,sizeof(T),1);
    }

public:

    //The symbols for accidentals are:
    //# - sharp
    //b - flat
    //n - natural

    //in Hz
    int sample_rate;

    //in beats per minute
    double tempo;

    short channels;

    //Letters contained in these strings are all suffixed with the respective modifier when getting their frequency
    std::string sharps;
    std::string flats;

    std::string name;

    std::vector<int16_t> samples;

    Custom_Sound(std::string get_name,int get_sample_rate,double get_tempo,short get_channels,std::string get_sharps="",std::string get_flats="");

    void reset(std::string get_name,int get_sample_rate,double get_tempo,short get_channels,std::string get_sharps="",std::string get_flats="");

    //Convert the passed note frequency to Hz
    double get_note_frequency(std::string frequency_string);

    //Convert the passed note value to seconds
    double get_note_length(std::string length_string);

    //frequency_string should hold either a note letter, such as "A", or a number representing the frequency in Hz
    //length_string should hold either a note value, such as "half", or a number representing the length in seconds
    void add_note(std::string frequency_string,std::string length_string);

    void save(std::string path);
};

#endif
