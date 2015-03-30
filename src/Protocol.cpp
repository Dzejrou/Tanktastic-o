#include "Protocol.hpp"

sf::Packet& operator<<(sf::Packet& pack, const PROTOCOL& prot)
{
    return pack << static_cast<sf::Uint32>(prot);
}

sf::Packet& operator>>(sf::Packet& pack, PROTOCOL& prot)
{
    sf::Uint32 tmp;
    pack >> tmp;
    prot = static_cast<PROTOCOL>(tmp);
    return pack;
}

sf::Packet& operator<<(sf::Packet& pack, const sf::Vector2f& vec)
{
    return pack << vec.x << vec.y;
}

sf::Packet& operator>>(sf::Packet& pack, sf::Vector2f& vec)
{
    return pack >> vec.x >> vec.y;
}

sf::Packet& operator<<(sf::Packet& pack, const DIRECTION::dir& dir)
{
    return pack << static_cast<sf::Uint32>(dir);
}

sf::Packet& operator>>(sf::Packet& pack, DIRECTION::dir& dir)
{
    sf::Uint32 tmp;
    pack >> tmp;
    dir = static_cast<DIRECTION::dir>(tmp);
    return pack;
}
