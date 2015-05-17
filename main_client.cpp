#include "src/Client.hpp"
#include <iostream>
#include <string>

int main()
{
    std::string addr;
    int port;
    
    std::cout << "[Init] Address: ";
    std::cin >> addr;
    std::cout << "[Init] Port: ";
    std::cin >> port;

    sf::IpAddress ip_addr;

    if(addr == "local")
        ip_addr = sf::IpAddress::getLocalAddress();
    else if(addr == "public")
        ip_addr = sf::IpAddress::getPublicAddress();
    else if(addr == "lh" || addr == "localhost")
        ip_addr = sf::IpAddress{"127.0.0.1"};
    else if(addr == "mff")
        ip_addr = sf::IpAddress{"195.113.21.150"};
    else
        ip_addr = sf::IpAddress{addr};

    std::string name;
    std::cout << "[Init] Name (seven characters maximum): ";
    std::cin >> name;
    if(name.size() > 7)
        name = name.substr(0,7);
    
    /**
     * Note: Defining window in this scope so that the players decides
     *       to re-enter the game, the same window is used.
     * */
    sf::RenderWindow window{sf::VideoMode{800,700},"Tanktastic-o!"};
    bool rematch{false};
    do
    {
        rematch = false; // Avoid endless loop after one rematch.
        Client client{ip_addr.toString(), port, name, rematch, window};
        client.run();
    } while(rematch); // In case the player wants to immidietly rejoin.

    return 0;
}
