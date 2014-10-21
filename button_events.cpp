#include "button_events.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

bool Button_Events::handle_button_event(string button_event,Window* parent_window){
    bool window_opened_on_top=false;

    if(button_event.length()>0){
        if(button_event=="close_window"){
            if(parent_window!=0){
                parent_window->toggle_on(true,false);
            }
        }
        else if(boost::algorithm::starts_with(button_event,"open_window_")){
            boost::algorithm::erase_first(button_event,"open_window_");

            engine_interface.get_window(button_event)->toggle_on(true,true);
            window_opened_on_top=true;
        }
        else if(button_event=="quit"){
            engine_interface.quit();
        }
        else if(button_event=="configure_commands"){
            engine_interface.get_window("configure_commands")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="reset_default_commands"){
            engine_interface.game_commands.clear();
            engine_interface.load_data("game_command");
            engine_interface.save_game_commands();
        }
        else if(button_event=="show_data_location"){
            string save_path=engine_interface.get_save_path();

            vector<string> lines;
            int spacing_x=engine_interface.get_font("small")->spacing_x;
            int window_width=(int)floor((double)main_window.SCREEN_WIDTH*0.8);

            while(save_path.length()*spacing_x>window_width){
                int i=window_width/spacing_x;
                lines.push_back(string(save_path,0,i));
                save_path.erase(save_path.begin(),save_path.begin()+i);
            }

            if(save_path.length()>0){
                lines.push_back(save_path);
            }

            save_path="";

            for(int i=0;i<lines.size();i++){
                save_path+=lines[i]+"\n";
            }

            engine_interface.make_notice("Your save data is located here:\n\n"+save_path);
            window_opened_on_top=true;
        }
        else if(button_event=="start_game_window"){
            engine_interface.get_window("start_game")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="start_game"){
            engine_interface.close_all_windows();

            game.start();
        }
        else if(button_event=="stop_game"){
            engine_interface.close_all_windows();

            game.stop();

            engine_interface.get_window("main_menu")->toggle_on();
        }
        else if(button_event=="start_server"){
            engine_interface.close_all_windows();

            game.start();

            network.start_as_server();
        }
        else if(button_event=="server_list_window"){
            engine_interface.get_window("server_list")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="server_list_delete_window"){
            engine_interface.get_window("server_list_delete")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="server_list_edit_window"){
            engine_interface.get_window("server_list_edit")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="add_server_window"){
            Window* window=engine_interface.get_window("add_server");

            if(window->get_info_text(2).length()==0){
                window->set_info_text(2,Strings::num_to_string(network.port));
            }

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="add_server"){
            if(parent_window!=0){
                network.add_server(parent_window->get_info_text(0),parent_window->get_info_text(1),Strings::string_to_unsigned_long(parent_window->get_info_text(2)),parent_window->get_info_text(3));

                engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
                engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
                engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();

                handle_button_event("close_window",parent_window);
            }
        }
        else if(button_event=="edit_server"){
            if(parent_window!=0){
                network.edit_server(engine_interface.editing_server,parent_window->get_info_text(0),parent_window->get_info_text(1),Strings::string_to_unsigned_long(parent_window->get_info_text(2)),parent_window->get_info_text(3));

                engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
                engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
                engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();

                handle_button_event("close_window",parent_window);
            }
        }

        else if(boost::algorithm::starts_with(button_event,"configure_commands_")){
            boost::algorithm::erase_first(button_event,"configure_commands_");

            engine_interface.configure_command=Strings::string_to_long(button_event);
            engine_interface.get_window("configure_command")->toggle_on(true,true);
            window_opened_on_top=true;
        }

        else if(boost::algorithm::starts_with(button_event,"server_list_delete_")){
            boost::algorithm::erase_first(button_event,"server_list_delete_");

            network.remove_server(Strings::string_to_long(button_event));

            engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
            engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
            engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();
        }
        else if(boost::algorithm::starts_with(button_event,"server_list_edit_")){
            boost::algorithm::erase_first(button_event,"server_list_edit_");

            Window* window=engine_interface.get_window("edit_server");
            Server* server=network.get_server(Strings::string_to_long(button_event));

            if(server!=0){
                window->set_info_text(0,server->name);
                window->set_info_text(1,server->address);
                window->set_info_text(2,Strings::num_to_string(server->port));
                window->set_info_text(3,server->password);

                engine_interface.editing_server=Strings::string_to_long(button_event);

                window->toggle_on(true,true);
                window_opened_on_top=true;
            }
        }
        else if(boost::algorithm::starts_with(button_event,"server_list_")){
            boost::algorithm::erase_first(button_event,"server_list_");

            engine_interface.close_all_windows();

            network.set_server_target(Strings::string_to_long(button_event));

            network.start_as_client();
        }

        else{
            Log::add_error("Invalid button event: '"+button_event+"'");
        }
    }

    return window_opened_on_top;
}
