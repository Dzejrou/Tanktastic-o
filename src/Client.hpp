#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "Protocol.hpp"
#include "Tank.hpp"
#include "Projectile.hpp"

/**
 * Class representing a client connected to the game, it is responsible
 * for connection, user input handling, packet communication handling,
 * updating the game and rendering.
 */
class Client
{
    #define MAX_PLR 6
    using uint32 = sf::Uint32;
    public:
        Client(std::string, int, std::string, bool&, sf::RenderWindow&);
        void run();
        ~Client();
    private:
        void process(sf::Event);
        void update();
        void render();
        bool handle_packet(sf::Packet&);
        void move(int);
        bool is_valid(sf::Vector2f, int);
        DIRECTION::dir key_to_dir(sf::Keyboard::Key);

        const int                   plr_max_{MAX_PLR};

        sf::RenderWindow&           window_;
        sf::TcpSocket               socket_;
        sf::Clock                   clock_, shoot_cd_;
        sf::Time                    frame_time_{sf::seconds(1/60.f)};
        sf::Time                    last_time_{sf::seconds(0.f)};
        sf::Time                    cd_time_{sf::seconds(1.f)};
        std::unique_ptr<Tank>       plr_[MAX_PLR];
        std::vector<Projectile>     shots_;
        STATE                       state_{STATE::PLAYING};
        sf::Text                    lost_, press_, game_name_;
        sf::Font                    font_;
        sf::RectangleShape          back_;

        std::string                 address_, name_;
        int                         port_, id_;
        bool                        running_{true}, focused_{true};
        bool&                       rematch_;
};
