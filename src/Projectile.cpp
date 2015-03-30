#include "Projectile.hpp"

/**
 * Brief: Projectile class constructor, sets the projectile's position
 *        and direction and associates it with a player.
 * Param: Identification number of the player that shot this projectile.
 * Param: Position of the projectile.
 * Param: Direction in which the projectile was shot.
 */
Projectile::Projectile(int id, sf::Vector2f pos, DIRECTION::dir dir)
    : dir_{dir}, ball_{5}, id_{id}
{
    setPosition(pos);
    ball_.setFillColor(sf::Color::Black);
}

/**
 * Brief: Moves the projectile.
 * Note: The name update was chosen so that is is not to be confused with
 *       the method move(sf::Vector) inherited from sf::Transformable, since
 *       this method is used in the client's each iteration of the update
 *       method.
 */
void Projectile::update()
{
    sf::Vector2f dir_vec;
    float speed = 5.f;
    switch(dir_)
    { // Chose the base vector.
        case DIRECTION::UP:
            dir_vec.x = 0.f;
            dir_vec.y = -1.f;
            break;
        case DIRECTION::DOWN:
            dir_vec.x = 0.f;
            dir_vec.y = 1.f;
            break;
        case DIRECTION::LEFT:
            dir_vec.x = -1.f;
            dir_vec.y = 0.f;
            break;
        case DIRECTION::RIGHT:
            dir_vec.x = 1.f;
            dir_vec.y = 0.f;
            break;
        case DIRECTION::NO_DIR:
            std::cout << "[Error] NO_DIR in projectile. (#"
                << id_ << ")" << std::endl;
            break;
    }

    /* Calculate and set the new position. */
    setPosition(getPosition() + dir_vec * speed);
}

/**
 * Brief: Returns a rectangle representing the bounds of this projectile,
 *        used for collision checking.
 */
sf::FloatRect Projectile::get_bounds()
{
    sf::FloatRect tmp = ball_.getGlobalBounds();
    tmp.left = getPosition().x;
    tmp.top = getPosition().y;
    return tmp;
}

/**
 * Brief: Method inherited from sf::Drawable that is used when this object
 *        is being drawn by a sf::RenderTarget instance.
 * Param: Reference to the sf::RenderTarget instance that draws this object.
 * Param: State that is used when drawing, to be update by this object's
 *        transformation.
 */
void Projectile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(ball_,states);
}

/**
 * Brief: Returns the id of the player that shot this projectile, used
 *        in collision checking.
 */
int Projectile::get_id()
{
    return id_;
}
