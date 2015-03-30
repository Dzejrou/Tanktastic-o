#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "Protocol.hpp"

/**
 * Tank class associated with a player, used to draw all sprites and texts
 * like name, score or the tank itself. Inherits from sf::Drawable so it
 * is able to be drawn and sf::Transformable so it is able to have it's
 * position set and the transformation used whilst being drawn.
 */
class Tank : public sf::Drawable, public sf::Transformable
{
    public:
        Tank(int, int);
        void set_dir(DIRECTION::dir);
        void add_point();
        void set_name(std::string);
        void set_moving(bool);
        sf::FloatRect get_bounds();
        int get_id();
        int get_score();
        bool is_moving();
        std::string get_name();
        DIRECTION::dir get_dir();
    private:
        void draw(sf::RenderTarget&, sf::RenderStates) const;

        int                         id_, score_;
        bool                        moving_{false};
        std::string                 name_;
        sf::Font                    font_;
        sf::Text                    name_text_, score_text_;
        std::array<sf::Sprite, 4>   spr_;
        std::array<sf::Texture, 4>  tex_;
        std::array<sf::Color, 4>    col_;
        DIRECTION::dir              dir_;
};
