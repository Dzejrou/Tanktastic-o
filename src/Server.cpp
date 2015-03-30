#include "Server.hpp"

/**
 * Brief: Construcs the server object and opens a listener on the given
 *        address and port.
 * Param: Address of this server.
 * Param: Port which the players will connect to.
 */
Server::Server(std::string a, int p)
    : address_{a}, port_{p}
{
    listener_.listen(port_);
    selector_.add(listener_);
}

/**
 * Brief: Server loop, checks for new connections and packets from players.
 */
void Server::run()
{
    int tmp_id;
    while(running_)
    {
        if(selector_.wait()) // Waiting for data.
        {
            if(selector_.isReady(listener_)) // Check for a pending connection.
            {
                std::cout << "Listener ready." << std::endl;
                std::unique_ptr<client> tmp_client{new client};
                std::cout << "Accepting him." << std::endl;
                if(listener_.accept(tmp_client->socket) == sf::Socket::Done)
                {
                    std::cout << "Connection established." << std::endl;
                    if((tmp_id = get_new_id()) != -1) // Free plr slots.
                    {
                        /* A check: */
                        if(plr_[tmp_id] != nullptr)
                            std::cout << "[Error] Gave an ID"
                                << " that is in use." << std::endl;

                        /* Add the client to the client list. */
                        tmp_client->id = tmp_id;
                        selector_.add(tmp_client->socket);
                        clients_.push_back(std::move(tmp_client));

                        init(*(clients_.back()));
                        std::cout << "Player with id=" << tmp_id
                            << " has joined the game." << std::endl;
                        tmp_id = -1;
                    }
                    else
                        std::cout << "[Error] User trying to connect when"
                            << " capacity is exceeded." << std::endl;
                }
            }
            else if(running_)// Check the clients.
            {
                for(const auto& client : clients_)
                {
                    if(selector_.isReady(client->socket))
                    {
                        sf::Packet packet;
                        if(client->socket.receive(packet) == sf::Socket::Done)
                        {
                            handle_client(packet, client->socket);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Brief: Handles new client connections.
 * Param: Reference to the new client.
 */
void Server::init(client& c)
{
    /* Set the position of the new player based in his id. */
    sf::Vector2f vec;
    switch(c.id)
    {
        case 0:
            vec.x = 50.f;
            vec.y = 50.f;
            break;
        case 1:
            vec.x = 700.f;
            vec.y = 50.f;
            break;
        case 2:
            vec.x = 700.f;
            vec.y = 500.f;
            break;
        case 3:
            vec.x = 50.f;
            vec.y = 500.f;
            break;
        default:
            std::cout << "[Error] Invalid player ID: " << c.id
                << std::endl;
    }
    plr_[c.id].reset(new Tank(c.id, 0));
    plr_[c.id]->setPosition(vec);

    /* Init message. */
    PROTOCOL head = PROTOCOL::START;
    sf::Packet packet;
    packet << head << static_cast<uint32>(c.id) << vec;
    c.socket.send(packet);
    packet.clear();

    /* Send info about the others. */
    head = PROTOCOL::PLR_SPAWNED;
    for(int i = 0; i < plr_max_; ++i)
    {
        if(plr_[i] == nullptr || i == c.id)
            continue; // Do not send to this one or nonexistent one.
        packet << head << static_cast<uint32>(i) // Send their positions.
            << plr_[i]->getPosition()
            << static_cast<uint32>(plr_[i]->get_score())
            << plr_[i]->is_moving();
        c.socket.send(packet);
        packet.clear();

        /* Inform about the direction of the plr aswell. */
        packet << PROTOCOL::DIRECTION_CHANGE << i << plr_[i]->get_dir();
        c.socket.send(packet);
        packet.clear();

        /* Send the names of the other players. */
        packet << PROTOCOL::PLR_NAME << static_cast<uint32>(i);
        packet << plr_[i]->get_name();
        c.socket.send(packet);
        packet.clear();
    }

    /* Send info about this player to the others. */
    packet.clear();
    packet << head << static_cast<uint32>(c.id) << vec
        << "" << static_cast<uint32>(0)
        << plr_[c.id]->is_moving();
    send_all(packet, c.id);
}

/**
 * Brief: Handles a single packet from a client.
 * Param: The incoming packet.
 * Param: Socket which the packet came from.
 */
void Server::handle_client(sf::Packet& packet, sf::TcpSocket& sock)
{
    PROTOCOL head;
    uint32 id32;
    packet >> head >> id32;
    int id = static_cast<int>(id32); // For better code readability.
    switch(head)
    {
        case PROTOCOL::MESSAGE:
        {
            std::string msg;
            packet >> msg;
            std::cout << msg << std::endl;
            break;
        }
        case PROTOCOL::PLR_MOVEMENT_CHANGE:
        {
            bool mov;
            packet >> mov;
            plr_[id]->set_moving(mov);

            /* Resend it. */
            packet.clear();
            packet << head << id32 << mov;
            send_all(packet, id);
            break;
        }
        case PROTOCOL::DIRECTION_CHANGE:
        {
            DIRECTION::dir dir;
            packet >> dir;

            if(plr_[id] != nullptr)
                plr_[id]->set_dir(dir);
            else // This should NOT happen.
                std::cout << "[Error] Packet from user that is not connected."
                    << std::endl;

            /* Let others know. */
            packet.clear();
            packet << head << id32 << dir;
            send_all(packet, id);
            break;
        }
        case PROTOCOL::PROJECTILE_SPAWNED:
        { // Projectiles handled on client side. Just resend it.
            sf::Vector2f pos;
            DIRECTION::dir dir;
            packet >> pos >> dir;

            // Repack it.
            packet.clear();
            packet << head << id32 << pos << dir;
            send_all(packet, id);
            break;
        }
        case PROTOCOL::PLR_DESTROYED:
        { // Delete player and inform others.
            uint32 id_killer;
            packet >> id_killer;
            plr_[id].reset(nullptr);
            std::cout << "Player with id=" << id
                << " was killed by player with id="
                << static_cast<int>(id_killer)
                << std::endl;

            packet.clear();
            packet << head << id32 << id_killer;
            send_all(packet, id); // Inform others.

            /* Increment the score for new players. */
            plr_[static_cast<int>(id_killer)]->add_point();
            break;
        }
        case PROTOCOL::PLR_QUIT:
        { // TODO: Time-out.
            plr_[id].reset(nullptr); // Get rid of him.
            
            // Get rid of the client.
            auto it = clients_.begin();
            while(it != clients_.end())
            {
                /**
                 * Note: I do not break if the client
                 *       is found, in case there was somehow a
                 *       duplicate.
                 */
                if((*it)->id == id) // Found it!
                    it = clients_.erase(it);
                else
                    ++it;
                std::cout << "looking for client to delete" << std::endl;
            }
            std::cout << "Finished deleting client." << std::endl;

            packet.clear();
            packet << head << id32;
            send_all(packet, id); // Inform others.
            break;
        }
        case PROTOCOL::PLR_NAME:
        { // Not used atm.
            std::string name;
            packet >> name;
            plr_[id]->set_name(name);

            packet.clear();
            packet << head << id32 << name;
            send_all(packet, id);
            break;
        }
        case PROTOCOL::PLR_NEW_POSITION:
        { // Just update the position, don't resend.
            sf::Vector2f pos;
            packet >> pos;
            plr_[id]->setPosition(pos);
        }
        default:
            // Do nothing.
            break;
    }
}

/**
 * Brief: Sends a packet to everybody except a given player.
 * Param: Packet to be sent.
 * Param: Id of the player to be excluded from the communication.
 */
void Server::send_all(sf::Packet& packet, int id)
{
    for(std::size_t i = 0; i < clients_.size(); ++i) // Need index.
    {
        if(clients_[i]->id != id)
            clients_[i]->socket.send(packet);
    }
}

/**
 * Brief: Returns a new id for a player or -1 if the server is full.
 */
int Server::get_new_id()
{
    for(int i = 0; i < plr_max_; ++i)
    {
        if(plr_[i] == nullptr)
            return i;
    }
    return -1; // No free space.
}
