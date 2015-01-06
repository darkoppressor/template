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

    //Per channel volumes, waveforms, and frequencies
    std::vector<double> volumes;
    std::vector<std::string> waveforms;
    std::vector<std::string> frequencies;

    std::vector<int16_t> samples;

    Custom_Sound(std::string get_name,int get_sample_rate,double get_tempo,short get_channels,std::string get_sharps="",std::string get_flats="");

    void reset(std::string get_name,int get_sample_rate,double get_tempo,short get_channels,std::string get_sharps="",std::string get_flats="");

    template<typename... Args>
    void set_volumes(Args const&... args){
        volumes.clear();

        std::vector<double> volume_values;

        int unpack[]{0,(volume_values.push_back(args),0)...};

        static_cast<void>(unpack);

        for(int i=0;i<channels;i++){
            if(i<volume_values.size()){
                volumes.push_back(volume_values[i]);
            }
            else{
                volumes.push_back(0.5);
            }
        }
    }

    template<typename... Args>
    void set_waveforms(Args const&... args){
        waveforms.clear();

        std::vector<std::string> waveform_strings;

        int unpack[]{0,(waveform_strings.push_back(args),0)...};

        static_cast<void>(unpack);

        for(int i=0;i<channels;i++){
            if(i<waveform_strings.size()){
                waveforms.push_back(waveform_strings[i]);
            }
            else{
                waveforms.push_back("sine");
            }
        }
    }

    template<typename... Args>
    void set_frequencies(Args const&... args){
        frequencies.clear();

        std::vector<std::string> frequency_strings;

        int unpack[]{0,(frequency_strings.push_back(args),0)...};

        static_cast<void>(unpack);

        for(int i=0;i<channels;i++){
            if(i<frequency_strings.size()){
                frequencies.push_back(frequency_strings[i]);
            }
            else{
                frequencies.push_back("C");
            }
        }
    }

    //Convert the passed note frequency to Hz
    double get_note_frequency(std::string frequency_string);

    //Convert the passed note value to seconds
    double get_note_length(std::string length_string);

    //Pass an empty string for frequency_string to use the currently set frequencies for each channel. Pass either a note letter, such as "A", or a number representing the frequency in Hz
    //to specify a different frequency (for all channels)
    //length_string should hold either a note value, such as "half", or a number representing the length in seconds
    //Pass "off" for waveform_override to use the currently set waveforms for each channel. Pass a waveform string to specify a different waveform (for all channels)
    //Pass a negative number for volume_override to use the currently set volumes for each channel. Pass a positive volume value to specify a different volume (for all channels)
    void add_note(std::string frequency_string,std::string length_string,std::string waveform_override="off",double volume_override=-1.0);

    void save(std::string path);
};

#endif
