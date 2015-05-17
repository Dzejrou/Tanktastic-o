#pragma once

#include <SFML/Network.hpp>

/**
 * #Brief this is the enum class used for the communication protocol
 *  used in Tanktastic-o project. Expected arguments in the packet are
 *  written in the comment after the enum constant.
 */
enum class PROTOCOL
{
    MESSAGE, // String MSG.
    START, // sf::Uint32 ID, sf::Vector2f POS.
    PLR_DESTROYED, // sf::Uint32 ID_KILLED, sf::Uint32 ID_KILLER.
    PLR_SPAWNED, // sf::Uint32 ID, sf::Vector2f POS, std::string NAME,
                 // sf::Uint32 SCORE, bool MOVING.
    PLR_QUIT, // sf::Uint32 ID.
    PLR_NAME, // sf::Uint32 ID, std::string NAME.
    PLR_MOVEMENT_CHANGE, // sf::Uint32 ID, bool MOVING.
    PLR_NEW_POSITION, // sf::Uint32 ID, sf::Vector2f POS.
    PROJECTILE_SPAWNED, // sf::Uint32 SHOOTER_ID, sf::Vector2f POS, DIRECTION D.
    DIRECTION_CHANGE, // sf::Uint32 ID, DIRECTION D.
    AFK_CHECK, // sf::Uint32 ID (DUMMY).
    AFK_RESPONSE // sf::Uint32 ID.
};

enum class STATE
{
    PLAYING, PAUSED, ENDED
};

namespace DIRECTION // Using enum inside a namespace for implicit integer
{                   // conversion which enum class does not provide.
    enum dir
    {
        UP = 0, DOWN, LEFT, RIGHT, NO_DIR
    };
}

sf::Packet& operator<<(sf::Packet&, const PROTOCOL&);
sf::Packet& operator>>(sf::Packet&, PROTOCOL&);
sf::Packet& operator<<(sf::Packet&, const sf::Vector2f&);
sf::Packet& operator>>(sf::Packet&, sf::Vector2f&);
sf::Packet& operator<<(sf::Packet&, const DIRECTION::dir&);
sf::Packet& operator>>(sf::Packet&, DIRECTION::dir&);
