#include "image.h"
#include "world.h"
#include "render.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Image::set_error_image(){
    error_image=get_image("error");
}

void Image::add_image(string name,SDL_Surface* surface){
    images.push_back(Image_Data());

    images[images.size()-1].load_image(surface);

    image_names.push_back(name);
}

void Image::load_images(){
    error_image=0;

    //Look through all of the files in the directory.
    for(File_IO_Directory_Iterator it("data/images");it.evaluate();it.iterate()){
        //If the file is not a directory.
        if(it.is_regular_file()){
            string file_name=it.get_file_name();

            boost::algorithm::trim(file_name);

            image_names.push_back(file_name);
        }
    }

    for(int i=0;i<image_names.size();i++){
        images.push_back(Image_Data());

        images[images.size()-1].load_image("data/images/"+image_names[i]);

        image_names[i].erase(image_names[i].end()-4,image_names[i].end());
    }
}

void Image::unload_images(){
    for(int i=0;i<images.size();i++){
        images[i].unload_image();
    }

    images.clear();
    image_names.clear();

    error_image=0;
}

Image_Data* Image::get_image(string image_name){
    Image_Data* ptr_image=0;

    for(int i=0;i<image_names.size();i++){
        if(image_names[i]==image_name){
            ptr_image=&images[i];

            break;
        }
    }

    if(ptr_image==0){
        Log::add_error("Error accessing image '"+image_name+"'");

        ptr_image=error_image;

        if(ptr_image==0){
            Log::add_error("Error accessing error image! Really?!");
        }
    }

    return ptr_image;
}
