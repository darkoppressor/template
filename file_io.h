/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef file_io_h
#define file_io_h

#include <string>
#include <sstream>

#include <SDL.h>

#ifdef GAME_OS_ANDROID
    #include <vector>
    #include <stdint.h>
    #include <dirent.h>
    #include <sys/stat.h>
#else
    #include <boost/filesystem.hpp>
#endif

const std::string SAVE_SUFFIX_TEMPORARY="_TEMPORARY";
const std::string SAVE_SUFFIX_BACKUP="_BACKUP";

class File_IO_Load{
public:

    bool load_success;
    std::istringstream data;

    File_IO_Load();
    File_IO_Load(std::string path);

    void load_file(std::string path);

    bool file_loaded();
    bool eof();

    void getline(std::string* line);
    std::string get_data();
};

class File_IO_Binary_Load{
public:

    bool load_success;
    std::string data;

    File_IO_Binary_Load();
    File_IO_Binary_Load(std::string path);

    void load_file(std::string path);

    bool file_loaded();

    std::string get_data();
};

class File_IO_Save{
public:

    SDL_RWops* rwops;

    std::string path;

    File_IO_Save(std::string get_path,bool append=false,bool binary=false);

    void open(std::string get_path,bool append,bool binary);
    bool close();

    bool is_opened();

    bool write(const void* ptr,size_t data_size,size_t data_count);
};

class File_IO{
private:

    static bool save_file(std::string path,std::string data,bool append,bool binary);

public:
    //First saves to a temporary file, then renames that file to the final filename
    static bool save_atomic(std::string path,std::string data,bool backup=false,bool append=false,bool binary=false);

    static bool exists(std::string path);
    static bool is_directory(std::string path);
    static bool is_regular_file(std::string path);
    static void create_directory(std::string path);
    static bool rename_file(std::string old_path,std::string new_path);
    //This does NOT overwrite new_path if it already exists
    static bool copy_file(std::string old_path,std::string new_path);
    static bool remove_file(std::string path);
    static void remove_directory(std::string path);
    static std::string get_file_name(std::string path);

    #ifdef GAME_OS_ANDROID
        static bool external_storage_available();
    #endif
};

class File_IO_Directory_Iterator{
public:

    #ifdef GAME_OS_ANDROID
        std::string directory;
        std::vector<std::string> asset_list;
        uint32_t entry;
    #else
        boost::filesystem::directory_iterator it;
    #endif

    File_IO_Directory_Iterator(std::string get_directory);

    bool evaluate();
    void iterate();
    bool is_directory();
    bool is_regular_file();
    std::string get_full_path();
    std::string get_file_name();
};

#ifdef GAME_OS_ANDROID
    class File_IO_Directory_Iterator_User_Data{
    public:

        std::string directory;
        DIR* dir;
        struct dirent* dir_entry;
        uint32_t entry;
        uint32_t entries;

        File_IO_Directory_Iterator_User_Data(std::string get_directory);
        ~File_IO_Directory_Iterator_User_Data();

        bool evaluate();
        void iterate();
        bool exists();
        bool is_regular_file();
        bool is_directory();
        std::string get_full_path();
        std::string get_file_name();
    };
#endif

#endif
