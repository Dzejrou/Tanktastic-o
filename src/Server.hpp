#pragma once

#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Protocol.hpp"
#include "Tank.hpp"

struct client; // Forward declaration, see at the bottom.

class Server
{
    using uint32 = sf::Uint32;
    public:
        Server(std::string, int);
        void run();
        void handle_client(sf::Packet&, sf::TcpSocket&);
        void init(client&);
        void send_all(sf::Packet&, int);
        ~Server() { listener_.close(); }
    private:
        int get_new_id();
        
        sf::SocketSelector                      selector_;
        sf::TcpListener                         listener_;
        std::vector<std::unique_ptr<client>>    clients_;
        std::unique_ptr<Tank>                   plr_[4];

        std::string                             address_;
        int                                     port_, plr_max_{4};
        bool                                    running_{true};
};

struct client
{
    sf::TcpSocket socket;
    int id;
};
