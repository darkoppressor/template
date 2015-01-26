#ifndef cipher_h
#define cipher_h

#include "rng.h"

#include <string>
#include <vector>
#include <stdint.h>

#include <SDL.h>
#include <SDL_mixer.h>

class Cipher{
private:

    //List of characters allowed by the cipher
    static const std::vector<unsigned char> characters;

    std::vector<uint32_t> seeds;

    //Generate the allowed characters
    static std::vector<unsigned char> generate_characters();
    //Returns the last character index
    static int get_max_character_index();
    //Converts a passed character into an index into the character list
    static int get_character_index(unsigned char input);
    //Converts a passed character index into a character
    static unsigned char get_character(int input);

    //Returns a list of RNG's generated from the seeds
    //This list should only be used once in a cipher
    std::vector<RNG> generate_rngs();

    unsigned char encipher_character(unsigned char character,std::vector<RNG>& rngs);
    unsigned char decipher_character(unsigned char character,std::vector<RNG>& rngs);
    void cipher(std::string& input,bool enciphering);
    std::string cipher_copy(std::string input,bool enciphering);

    void encipher_pixel(SDL_Surface* surface,int x,int y,std::vector<RNG>& rngs);
    void decipher_pixel(SDL_Surface* surface,int x,int y,std::vector<RNG>& rngs);
    void cipher(SDL_Surface* input,bool enciphering);

    uint8_t encipher_sound_byte(uint8_t sound_byte,std::vector<RNG>& rngs);
    uint8_t decipher_sound_byte(uint8_t sound_byte,std::vector<RNG>& rngs);
    void cipher(Mix_Chunk* input,bool enciphering);

public:

    Cipher(const std::vector<std::string>& seed_strings);

    void set_seeds(const std::vector<std::string>& seed_strings);

    void encipher(std::string& input);
    void decipher(std::string& input);
    std::string encipher_copy(const std::string& input);
    std::string decipher_copy(const std::string& input);

    void encipher(SDL_Surface* input);
    void decipher(SDL_Surface* input);

    void encipher(Mix_Chunk* input);
    void decipher(Mix_Chunk* input);
};

#endif
