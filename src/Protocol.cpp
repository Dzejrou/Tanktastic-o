#include "Protocol.hpp"

/**
 * Brief: Allows native sending of PROTOCOL head messages in packets.
 * Param: The packet.
 * Param: Cont reference to the protocol head message.
 */
sf::Packet& operator<<(sf::Packet& pack, const PROTOCOL& prot)
{
    return pack << static_cast<sf::Uint32>(prot);
}

/**
 * Brief: Allows native sending of PROTOCOL head messages in packets.
 * Param: The packet.
 * Param: Cont reference to the protocol head message.
 */
sf::Packet& operator>>(sf::Packet& pack, PROTOCOL& prot)
{
    sf::Uint32 tmp;
    pack >> tmp;
    prot = static_cast<PROTOCOL>(tmp);
    return pack;
}

/**
 * Brief: Allows native sending of vectors in packets.
 * Param: The packet.
 * Param: Vector to be sent. (sf::Vector2f)
 */
sf::Packet& operator<<(sf::Packet& pack, const sf::Vector2f& vec)
{
    return pack << vec.x << vec.y;
}

/**
 * Brief: Allows native sending of vectors in packets.
 * Param: The packet.
 * Param: Vector to be sent. (sf::Vector2f)
 */
sf::Packet& operator>>(sf::Packet& pack, sf::Vector2f& vec)
{
    return pack >> vec.x >> vec.y;
}

/**
 * Brief: Allows native sending of directions in packets.
 * Param: The packet.
 * Param: Const reference to the direction being sent.
 */
sf::Packet& operator<<(sf::Packet& pack, const DIRECTION::dir& dir)
{
    return pack << static_cast<sf::Uint32>(dir);
}

/**
 * Brief: Allows native sending of directions in packets.
 * Param: The packet.
 * Param: Const reference to the direction being sent.
 */
sf::Packet& operator>>(sf::Packet& pack, DIRECTION::dir& dir)
{
    sf::Uint32 tmp;
    pack >> tmp;
    dir = static_cast<DIRECTION::dir>(tmp);
    return pack;
}
