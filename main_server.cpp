#include "src/Server.hpp"
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
    else
        ip_addr = sf::IpAddress{addr};

    std::cout << "[Init] Chosen address: "
        << ip_addr.toString() << std::endl << "[Init] Chosen port: "
        << port << std::endl;

    Server server{ip_addr.toString(),port};
    server.run();

    return 0;
}
