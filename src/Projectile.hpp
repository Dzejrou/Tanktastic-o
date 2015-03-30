#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Protocol.hpp"

/**
 * Class representing a single projectile shot by the player.
 * Inherits from sf::Drawable so it is able to be drawn and
 * sf::Transformable, so it can modify the rendering state with
 * it's current position.
 */
class Projectile : public sf::Drawable, public sf::Transformable
{
    public:
        Projectile(int, sf::Vector2f, DIRECTION::dir);
        void update();
        sf::FloatRect get_bounds();
        int get_id();
    private:
        void draw(sf::RenderTarget&, sf::RenderStates) const;

        DIRECTION::dir      dir_;
        sf::CircleShape     ball_;
        int                 id_;
};
