/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "network_game.h"

#include <network_lockstep.h>

using namespace std;

bool Network_Game::receive_game_packet(RakNet::Packet* packet,const RakNet::MessageID& packet_id){
    /**if(packet_id==ID_GAME_EXAMPLE){
        ///Do something with this packet

        return true;
    }*/

    return false;
}

string Network_Game::allow_new_connection(){
    /**if(SOME_EXAMPLE_REASON_TO_REJECT_NEW_CONNECTIONS){
        return "Example reason";
    }*/

    return "";
}

void Network_Game::write_initial_game_data(RakNet::BitStream* bitstream){
    ///bitstream->WriteCompressed();
}

void Network_Game::read_initial_game_data(RakNet::BitStream* bitstream){
    ///bitstream->ReadCompressed();
}

void Network_Game::write_update(RakNet::BitStream* bitstream){
    ///bitstream->WriteCompressed();
}

void Network_Game::read_update(RakNet::BitStream* bitstream){
    ///bitstream->ReadCompressed();
}

void Network_Game::write_server_ready(RakNet::BitStream* bitstream){
    bitstream->WriteCompressed(Network_Lockstep::server_turn_complete);

    ///Write game commands
}

void Network_Game::read_server_ready(RakNet::BitStream* bitstream){
    bitstream->ReadCompressed(Network_Lockstep::server_turn_complete);

    ///Read game commands
}
