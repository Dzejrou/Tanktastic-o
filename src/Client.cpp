#include "Client.hpp"

/**
 * Brief: Constructs the client objects, connects to the server and
 *        completes initial communication.
 * Param: Address of the server.
 * Param: Port to be connected to on the server.
 * Param: Name of the player.
 * Param: Reference to the rematch-checking boolean.
 * Param: Reference to the game window.
 */
Client::Client(std::string a, int p, std::string n, bool& rematch,
               sf::RenderWindow& window)
    : window_{window}, address_{a}, name_{n}, port_{p},
      rematch_{rematch}
{
    /* Connect to the server. */
    socket_.connect(address_,port_);

    window_.setKeyRepeatEnabled(false); // Do not interpret pressed key for
                                        // multiple presses.

    /* Load the font and set the 'You lost.' text. */
    font_.loadFromFile("Resources/Inconsolata.otf");
    lost_.setString("You lost!");
    lost_.setFont(font_);
    lost_.setCharacterSize(60);
    lost_.setColor(sf::Color::Black);
    lost_.setPosition(sf::Vector2f(280, 250.f));

    /* Set the 'Press ... to quit.' text. */
    press_.setString("Press space to rematch.\nPress any other key to quit.");
    press_.setFont(font_);
    press_.setCharacterSize(25);
    press_.setColor(sf::Color::Black);
    press_.setPosition(sf::Vector2f(250.f, 300 + 30.f));

    /* Set the bottom black bar. */
    back_.setFillColor(sf::Color::Black);
    back_.setPosition(sf::Vector2f(0.f, 600.f));
    back_.setSize(sf::Vector2f(800.f, 100.f));

    /* Set the game name banner at the bottom. */
    game_name_.setString("TANKTASTIC-O");
    game_name_.setFont(font_);
    game_name_.setCharacterSize(55);
    game_name_.setColor(sf::Color::White);
    game_name_.setPosition(450.f, 615.f);

    /* Initial conversation with the server. */
    sf::Packet packet;
    if(socket_.receive(packet) == sf::Socket::Done)
    {
        PROTOCOL head;
        packet >> head;
        if(head == PROTOCOL::START)
        {
            std::cout << "[Status] Got START info." << std::endl;
            /* Get the id and position. */
            uint32 tmp;
            sf::Vector2f position;
            packet >> tmp;
            id_ = static_cast<int>(tmp);
            std::cout << "[Status] Got ID #" << id_ << "." << std::endl;
            packet >> position;
            plr_[id_].reset(new Tank(id_, 0));
            plr_[id_]->setPosition(position);
            plr_[id_]->set_name(n);
            packet.clear();

            /* Send the name to the server. */
            packet << PROTOCOL::PLR_NAME << tmp << name_;
            socket_.send(packet);
        }
        else
        {
            std::cout << "[Error] Wrong packet received." << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cout << "[Error] Server didn't send initial data." << std::endl;
        exit(1);
    }

    socket_.setBlocking(false); // Wait ONLY for the initial message.
}

/**
 * Brief: The game loop.
 */
void Client::run()
{
    sf::Packet packet;
    sf::Time elapsed_time;
    sf::Event event;
    while(window_.isOpen() && running_)
    {
        // Do this on every iteration, do not limit to FPS.
        if(socket_.receive(packet) == sf::Socket::Done)
        { // Handle info sent from the server.
            handle_packet(packet);
            packet.clear();
        }

        elapsed_time = clock_.restart();
        last_time_ += elapsed_time;
        while(last_time_ >= frame_time_) // 60 FPS.
        {
            // Main while loop blocked on higher fps, thus need to
            // do this again.
            if(socket_.receive(packet) == sf::Socket::Done)
            { // Handle info sent from the server.
                handle_packet(packet);
                packet.clear();
            }

            last_time_ -= frame_time_;
            if(window_.pollEvent(event) && state_ != STATE::PAUSED)
            { // Handle player input when game isn't paused. 
                process(event);
            }

            if(state_ != STATE::ENDED)
            { // Upon death the game should freeze.
                update();
                render();
            }
        }
    }
}

/**
 * Brief: Processes the given event and if its a key related one,
 *        handles the input.
 * Param: Event that occured.
 */
void Client::process(sf::Event event)
{
    sf::Packet packet;
    if(event.type == sf::Event::Closed)
        running_ = false;
    else if(state_ == STATE::PLAYING)
    {
        switch(event.type)
        {
            // These two keep track of the fact whether the player's
            // window is focused or not - if the game should handle input
            // when there are more instances or the player is in a different
            // window.
            case sf::Event::GainedFocus:
                focused_ = true;
                break;
            case sf::Event::LostFocus:
                focused_ = false;

                if(plr_[id_]->is_moving())
                {
                    packet.clear();
                    packet << PROTOCOL::PLR_MOVEMENT_CHANGE
                        << static_cast<uint32>(id_) << false;
                    socket_.send(packet);
                    plr_[id_]->set_moving(false);
                }
                break;
            case sf::Event::KeyReleased:
                if(!focused_)
                    break;
                if(plr_[id_]->is_moving() &&
                        plr_[id_]->get_dir() == key_to_dir(event.key.code))
                {
                    plr_[id_]->set_moving(false);
                    packet.clear();
                    packet << PROTOCOL::PLR_MOVEMENT_CHANGE
                        << static_cast<uint32>(id_)
                        << false;
                    socket_.send(packet);
                }
                break;
            case sf::Event::KeyPressed:
                if(!focused_)
                    break;
                switch(event.key.code)
                {
                    case sf::Keyboard::W:
                    case sf::Keyboard::S:
                    case sf::Keyboard::A:
                    case sf::Keyboard::D:
                        if(!plr_[id_]->is_moving())
                        { // Start moving and inform the server.
                            plr_[id_]->set_moving(true);
                            packet.clear();
                            packet << PROTOCOL::PLR_MOVEMENT_CHANGE
                                << static_cast<uint32>(id_)
                                << true;
                            socket_.send(packet);
                        }
                        if(plr_[id_]->get_dir() !=
                                key_to_dir(event.key.code))
                        {
                            // Change the direction and inform
                            // the server.
                            plr_[id_]->set_dir(key_to_dir(event.key.code));
                            packet.clear();
                            packet << PROTOCOL::DIRECTION_CHANGE
                                << static_cast<uint32>(id_)
                                << plr_[id_]->get_dir();
                            socket_.send(packet);
                        }
                        break;
                    case sf::Keyboard::Space:
                        if(shoot_cd_.getElapsedTime() > cd_time_)
                        { // Create a new projectile.
                            shots_.push_back(Projectile{id_,
                                    plr_[id_]->getPosition(),
                                    plr_[id_]->get_dir()});

                            /* Inform the server. */
                            packet.clear();
                            packet << PROTOCOL::PROJECTILE_SPAWNED
                                << static_cast<uint32>(id_)
                                << plr_[id_]->getPosition()
                                << plr_[id_]->get_dir();
                            socket_.send(packet);
                            shoot_cd_.restart();
                        }
                        break;
                    default:
                        // Do nothing.
                        break;
                }
                break;
            default:
                // Do nothing.
                break;
        }
    }
    else if(state_ == STATE::ENDED)
    {
        if(event.type == sf::Event::KeyPressed)
        {
            std::cout << "[Status] Quiting game." << std::endl;
            running_ = false;
            if(event.key.code == sf::Keyboard::Space)
                rematch_ = true; // Tell the main method to re-enter the game.
        }
    }
}

/**
 * Brief: Converts a key to a direction.
 * Param: The key code.
 */
DIRECTION::dir Client::key_to_dir(sf::Keyboard::Key key)
{
    switch(key)
    {
        case sf::Keyboard::W:
            return DIRECTION::UP;
        case sf::Keyboard::S:
            return DIRECTION::DOWN;
        case sf::Keyboard::A:
            return DIRECTION::LEFT;
        case sf::Keyboard::D:
            return DIRECTION::RIGHT;
        default:
            return DIRECTION::NO_DIR; // Key combination.
    }
}

/**
 * Brief: Moves the given tank.
 * Param: Id of the tank.
 */
void Client::move(int id)
{
    sf::Vector2f new_position, delta_position;
    DIRECTION::dir dir = plr_[id]->get_dir();
    float speed = 3.f;
    switch(dir)
    {
        case DIRECTION::UP:
            delta_position.x = 0.f;
            delta_position.y = -1.f;
            break;
        case DIRECTION::DOWN:
            delta_position.x = 0.f;
            delta_position.y = 1.f;
            break;
        case DIRECTION::LEFT:
            delta_position.x = -1.f;
            delta_position.y = 0.f;
            break;
        case DIRECTION::RIGHT:
            delta_position.x = 1.f;
            delta_position.y = 0.f;
            break;
        case DIRECTION::NO_DIR:
        default:
            std::cout << "[Error] A wild NO_DIR appeared! (#" << id
                << " - " << dir << ")"<< std::endl;
    }

    /* Calculate the new position and update it. */
    new_position = plr_[id]->getPosition() + delta_position * speed;
    if(is_valid(new_position, id))
        plr_[id]->setPosition(new_position);

    if(id == id_) // It's me!
    {
        /* Inform the server about this new position. */
        sf::Packet packet;
        packet << PROTOCOL::PLR_NEW_POSITION << static_cast<uint32>(id_)
            << new_position;
        socket_.send(packet);
    }
}

/**
 * Brief: Checks if the given position is valid and can be moved on.
 * Param: Vector of the position.
 * Param: Id of the tank that is moving.
 */
bool Client::is_valid(sf::Vector2f position, int id)
{
    // Check the window bounds.
    if(position.x < 0 || position.x > 785 || position.y < 0 || position.y > 585)
        return false;
    for(int i = 0; i < plr_max_; ++i)
    {
        if(plr_[i] == nullptr || i == id)
            continue;
        if(plr_[i]->get_bounds().contains(position))
            return false;
    }
    return true;
}

/**
 * Brief: Updates the game.
 */
void Client::update()
{
    auto it = shots_.begin();
    while(it != shots_.end())
    {
        it->update();
        if(it->getPosition().x < 0 || it->getPosition().x > 800
                || it->getPosition().y < 0 || it->getPosition().y > 600)
            it = shots_.erase(it);
        else if(it->get_id() != id_ && 
                plr_[id_]->get_bounds().intersects(it->get_bounds()) &&
                state_ == STATE::PLAYING)
        {
            sf::Packet packet;
            packet << PROTOCOL::PLR_DESTROYED << static_cast<uint32>(id_)
                << static_cast<uint32>(it->get_id());
            socket_.send(packet);
            state_ = STATE::ENDED; // End the game.
            plr_[id_].reset(nullptr); // Kill the tank.
            return; // End of game on this side, continuing in the iteration
                    // might cause mess :)
        }
        else
            ++it;
    }

    for(int i = 0; i < plr_max_; ++i)
    {
        if(plr_[i] != nullptr && plr_[i]->is_moving())
        {
            move(i);
        }
    }
}

/**
 * Brief: Draws everything that is to be drawn.
 */
void Client::render()
{
    // Background.
    window_.clear(sf::Color::White);

    // Botom info rectangle.
    window_.draw(back_);
    window_.draw(game_name_);

    // Players.
    for(int i = 0; i < plr_max_; ++i)
    {
        if(plr_[i] != nullptr)
            window_.draw(*plr_[i]);
    }

    // Death text.
    if(state_ == STATE::ENDED)
    {
        window_.draw(lost_);
        window_.draw(press_);
    }

    // Projectiles.
    for(auto& shot : shots_)
        window_.draw(shot);

    // Draws it all to the window.
    window_.display();
}

/**
 * Brief: Handles a single packet from the server.
 * Param: The incoming packet.
 */
bool Client::handle_packet(sf::Packet& packet)
{
    /**
     * For information about the protocol heads see the file
     * Protocol.hpp or the documentation for it.
     */
    PROTOCOL    head;
    uint32      id32;
    int         id;
    packet >> head >> id32;
    id = static_cast<int>(id32);
    switch(head)
    {
        case PROTOCOL::PLR_SPAWNED:
        {
            sf::Vector2f pos;
            uint32 score32;
            bool mov;
            packet >> pos >> score32 >> mov;
            int score = static_cast<int>(score32);
            plr_[id].reset(new Tank(id, score));
            plr_[id]->setPosition(pos);
            plr_[id]->set_moving(mov);
            break;
        }
        case PROTOCOL::PLR_MOVEMENT_CHANGE:
        {
            bool mov;
            packet >> mov;
            plr_[id]->set_moving(mov);
            break;
        }
        case PROTOCOL::DIRECTION_CHANGE:
        {
            DIRECTION::dir dir;
            packet >> dir;
            plr_[id]->set_dir(dir);   
            break;
        }
        case PROTOCOL::PROJECTILE_SPAWNED:
        {
            sf::Vector2f pos;
            DIRECTION::dir dir;
            packet >> pos >> dir;

            shots_.push_back(Projectile{id,pos,dir});
            break;
        }
        case PROTOCOL::PLR_DESTROYED:
        {
            uint32 id_killer32;
            packet >> id_killer32;
            int id_killer = static_cast<int>(id_killer32);
            plr_[id].reset(nullptr);
            plr_[id_killer]->add_point();
            break;
        }
        case PROTOCOL::PLR_QUIT:
            plr_[id].reset(nullptr); // Everything else is done by server.
            break;
        case PROTOCOL::PLR_NAME:
        {
            std::string name;
            packet >> name;
            plr_[id]->set_name(name);
            break;
        }
        case PROTOCOL::AFK_CHECK:
        {
            packet.clear();

            // Send response - reuse the id.
            packet << PROTOCOL::AFK_RESPONSE << id32;
            socket_.send(packet);
            packet.clear();
            break;
        }
        case PROTOCOL::PLR_NEW_POSITION:
        {
            sf::Vector2f new_pos;
            packet >> new_pos;
            packet.clear();

            // Check and update the position if necessary.
            if(plr_[id] != nullptr && plr_[id]->getPosition() != new_pos
            && is_valid(new_pos, id))
                plr_[id]->setPosition(new_pos);
        }
        default:
            // Should not happen.
            break;
    }
    return false;
}

/**
 * Brief: Desctructor of the client class, informs the server and closes
 *        the connection.
 */
Client::~Client()
{
    sf::Packet packet;
    packet << PROTOCOL::PLR_QUIT << static_cast<int>(id_);
    socket_.send(packet);
    socket_.disconnect();
}
