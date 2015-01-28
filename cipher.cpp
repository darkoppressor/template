#include "cipher.h"
#include "pixels.h"

#include <boost/crc.hpp>

using namespace std;

const vector<unsigned char> Cipher::characters=Cipher::generate_characters();

vector<unsigned char> Cipher::generate_characters(){
    vector<unsigned char> temp_characters;

    for(unsigned char i=' ';i<='~';i++){
        temp_characters.push_back(i);
    }

    temp_characters.push_back('\n');

    return temp_characters;
}

int Cipher::get_max_character_index(){
    return characters.size()-1;
}

int Cipher::get_character_index(unsigned char input){
    for(int i=0;i<characters.size();i++){
        if(input==characters[i]){
            return i;
        }
    }

    return 0;
}

unsigned char Cipher::get_character(int input){
    if(input<0 || input>get_max_character_index()){
        input=0;
    }

    return characters[input];
}

vector<RNG> Cipher::generate_rngs(){
    vector<RNG> rngs;

    for(size_t i=0;i<seeds.size();i++){
        rngs.push_back(RNG(seeds[i]));
    }

    return rngs;
}

unsigned char Cipher::encipher_character(unsigned char character,vector<RNG>& rngs){
    int character_index=get_character_index(character);

    for(size_t i=0;i<rngs.size();i++){
        character_index+=(int)rngs[i].random_range(0,(uint32_t)get_max_character_index());

        while(character_index>get_max_character_index()){
            character_index-=get_max_character_index()+1;
        }
    }

    return get_character(character_index);
}

unsigned char Cipher::decipher_character(unsigned char character,vector<RNG>& rngs){
    int character_index=get_character_index(character);

    for(size_t i=rngs.size()-1;;i--){
        character_index-=(int)rngs[i].random_range(0,(uint32_t)get_max_character_index());

        while(character_index<0){
            character_index+=get_max_character_index()+1;
        }

        if(i==0){
            break;
        }
    }

    return get_character(character_index);
}

void Cipher::cipher(string& input,bool enciphering){
    if(seeds.size()>0){
        vector<RNG> rngs=generate_rngs();

        for(size_t i=0;i<input.length();i++){
            if(enciphering){
                input[i]=encipher_character(input[i],rngs);
            }
            else{
                input[i]=decipher_character(input[i],rngs);
            }
        }
    }
}

string Cipher::cipher_copy(string input,bool enciphering){
    cipher(input,enciphering);

    return input;
}

void Cipher::encipher_pixel(SDL_Surface* surface,int x,int y,vector<RNG>& rngs){
    Color color=surface_get_pixel(surface,x,y);
    int red=color.get_red_short();
    int green=color.get_green_short();
    int blue=color.get_blue_short();
    int alpha=color.get_alpha_short();

    for(size_t i=0;i<rngs.size();i++){
        red+=(int)rngs[i].random_range(0,255);
        green+=(int)rngs[i].random_range(0,255);
        blue+=(int)rngs[i].random_range(0,255);
        alpha+=(int)rngs[i].random_range(0,255);

        while(red>255){
            red-=256;
        }
        while(green>255){
            green-=256;
        }
        while(blue>255){
            blue-=256;
        }
        while(alpha>255){
            alpha-=256;
        }
    }

    color.set_rgb(red,green,blue,alpha);

    surface_put_pixel(surface,x,y,color);
}

void Cipher::decipher_pixel(SDL_Surface* surface,int x,int y,vector<RNG>& rngs){
    Color color=surface_get_pixel(surface,x,y);
    int red=color.get_red_short();
    int green=color.get_green_short();
    int blue=color.get_blue_short();
    int alpha=color.get_alpha_short();

    for(size_t i=rngs.size()-1;;i--){
        red-=(int)rngs[i].random_range(0,255);
        green-=(int)rngs[i].random_range(0,255);
        blue-=(int)rngs[i].random_range(0,255);
        alpha-=(int)rngs[i].random_range(0,255);

        while(red<0){
            red+=256;
        }
        while(green<0){
            green+=256;
        }
        while(blue<0){
            blue+=256;
        }
        while(alpha<0){
            alpha+=256;
        }

        if(i==0){
            break;
        }
    }

    color.set_rgb(red,green,blue,alpha);

    surface_put_pixel(surface,x,y,color);
}

void Cipher::cipher(SDL_Surface* input,bool enciphering){
    if(seeds.size()>0){
        vector<RNG> rngs=generate_rngs();

        for(int x=0;x<input->w;x++){
            for(int y=0;y<input->h;y++){
                if(enciphering){
                    encipher_pixel(input,x,y,rngs);
                }
                else{
                    decipher_pixel(input,x,y,rngs);
                }
            }
        }
    }
}

uint8_t Cipher::encipher_sound_byte(uint8_t sound_byte,vector<RNG>& rngs){
    int input=sound_byte;

    for(size_t i=0;i<rngs.size();i++){
        input+=(int)rngs[i].random_range(0,255);

        while(input>255){
            input-=256;
        }
    }

    return (uint8_t)input;
}

uint8_t Cipher::decipher_sound_byte(uint8_t sound_byte,vector<RNG>& rngs){
    int input=sound_byte;

    for(size_t i=rngs.size()-1;;i--){
        input-=(int)rngs[i].random_range(0,255);

        while(input<0){
            input+=256;
        }

        if(i==0){
            break;
        }
    }

    return (uint8_t)input;
}

void Cipher::cipher(Mix_Chunk* input,bool enciphering){
    if(seeds.size()>0){
        vector<RNG> rngs=generate_rngs();

        for(size_t i=0;i<input->alen;i++){
            if(enciphering){
                input->abuf[i]=encipher_sound_byte(input->abuf[i],rngs);
            }
            else{
                input->abuf[i]=decipher_sound_byte(input->abuf[i],rngs);
            }
        }
    }
}

Cipher::Cipher(const vector<string>& seed_strings){
    set_seeds(seed_strings);
}

void Cipher::set_seeds(const vector<string>& seed_strings){
    seeds.clear();

    for(size_t i=0;i<seed_strings.size();i++){
        if(seed_strings[i].length()>0){
            boost::crc_32_type result;
            result.process_bytes(seed_strings[i].data(),seed_strings[i].length());

            seeds.push_back((uint32_t)result.checksum());
        }
    }
}

void Cipher::encipher(string& input){
    cipher(input,true);
}

void Cipher::decipher(string& input){
    cipher(input,false);
}

string Cipher::encipher_copy(const string& input){
    return cipher_copy(input,true);
}

string Cipher::decipher_copy(const string& input){
    return cipher_copy(input,false);
}

void Cipher::encipher(SDL_Surface* input){
    cipher(input,true);
}

void Cipher::decipher(SDL_Surface* input){
    cipher(input,false);
}

void Cipher::encipher(Mix_Chunk* input){
    cipher(input,true);
}

void Cipher::decipher(Mix_Chunk* input){
    cipher(input,false);
}
