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

/**
 * Class representing the server in this game, controls the communication
 * between clients, provides time out checks and movement correction.
 */
class Server
{
    #define MAX_PLR 4
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
        void remove_client(int);
        void kick(std::unique_ptr<client>&);
        
        sf::SocketSelector                      selector_;
        sf::TcpListener                         listener_;
        sf::Clock                               disc_clock_, mov_check_clock_;
        std::vector<std::unique_ptr<client>>    clients_;
        std::unique_ptr<Tank>                   plr_[MAX_PLR];

        std::string                             address_;
        int                                     port_, plr_max_{MAX_PLR};
        bool                                    running_{true};
        bool                                    not_afk_[MAX_PLR];
        sf::Time                                time_out_, mov_check_;
};

struct client
{
    sf::TcpSocket socket;
    int id;
};
