#ifndef server_h
#define server_h

#include <string>

class Server{
public:

    std::string name;
    std::string address;
    unsigned short port;
    //For a direct IP server, this stores the password for the server
    //For a LAN browser server, if this has a non-zero length, a password is required
    std::string password;

    Server();
};

#endif
