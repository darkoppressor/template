#include "network.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Network::set_lan_server_target(int index,string get_password){
    if(index<lan_server_list.size()){
        server_address=lan_server_list[index].address;
        server_port=lan_server_list[index].port;
        server_password=get_password;
    }
}

void Network::add_lan_server(string get_name,string get_address,unsigned short get_port,string get_password){
    lan_server_list.push_back(Server());
    lan_server_list[lan_server_list.size()-1].name=get_name;
    lan_server_list[lan_server_list.size()-1].address=get_address;
    lan_server_list[lan_server_list.size()-1].port=get_port;
    lan_server_list[lan_server_list.size()-1].password=get_password;

    engine_interface.get_window("lan_server_list")->rebuild_scrolling_buttons();
}

Server* Network::get_lan_server(int index){
    Server* ptr_server=0;

    if(index<lan_server_list.size()){
        ptr_server=&lan_server_list[index];
    }

    return ptr_server;
}

void Network::lan_refresh(){
    if(status=="off"){
        start_as_server(false);
    }

    lan_server_list.clear();

    peer->Ping("255.255.255.255",port,false);

    engine_interface.get_window("lan_server_list")->rebuild_scrolling_buttons();
}

void Network::receive_lan_server(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    RakNet::TimeMS timestamp;
    bitstream.Read(timestamp);

    unsigned int data_length=packet->length-sizeof(RakNet::MessageID)-sizeof(RakNet::TimeMS);

    string name_and_password="";
    for(unsigned int i=0;i<data_length;i++){
        unsigned char character;
        bitstream.Read(character);
        name_and_password+=character;
    }
    bool password_needed=false;
    if(name_and_password.length()>0 && Strings::string_to_unsigned_long(string(1,name_and_password[0]))==1){
        password_needed=true;
    }

    vector<string> connection_data;
    string str_connection_data=packet->systemAddress.ToString(true);
    boost::algorithm::split(connection_data,str_connection_data,boost::algorithm::is_any_of("|"));

    if(connection_data.size()>=2 && name_and_password.length()>0){
        //Erase the password character from the string
        name_and_password.erase(name_and_password.begin());

        string str_password_needed="";
        if(password_needed){
            str_password_needed="1";
        }

        add_lan_server(name_and_password,connection_data[0],Strings::string_to_unsigned_long(connection_data[1]),str_password_needed);
    }
    else{
        Log::add_log("Failed to read received LAN server data: "+str_connection_data);
    }
}
