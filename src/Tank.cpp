#include "Tank.hpp"

/**
 * Brief: Tank class constructor, initializes variables and loads
 *        all needed resources.
 * Param: Identification number of the tank, 0-3.
 * Param: Score of the player associated with the tank.
 */
Tank::Tank(int id, int s)
    : id_{id}, dir_{static_cast<DIRECTION::dir>(id)}, score_{s}
{
    /* Set the colors for each player id. */
    col_[0] = sf::Color::Red;
    col_[1] = sf::Color::Blue;
    col_[2] = sf::Color::Green;
    col_[3] = sf::Color::Yellow;

    /* Load textures for all directions. */
    std::string id_s = std::to_string(id_);
    tex_[DIRECTION::UP].loadFromFile("Resources/up" + id_s + ".png");
    tex_[DIRECTION::DOWN].loadFromFile("Resources/down" + id_s + ".png");
    tex_[DIRECTION::LEFT].loadFromFile("Resources/left" + id_s + ".png");
    tex_[DIRECTION::RIGHT].loadFromFile("Resources/right" + id_s + ".png");

    /* Setup font and name text. */
    font_.loadFromFile("Resources/Inconsolata.otf");
    name_text_.setFont(font_);
    name_text_.setString(name_);
    name_text_.setColor(col_[id_]);
    name_text_.setCharacterSize(25);
    name_text_.setPosition(30.f + id_*100.f, 630.f);

    /* Setup the score text. */
    score_text_.setFont(font_);
    score_text_.setString(std::to_string(score_));
    score_text_.setColor(col_[id_]);
    score_text_.setCharacterSize(25);
    score_text_.setPosition(40.f + id_*100.f, 650.f);

    /* Associate textures with sprites. */
    for(int i = 0; i < 4; ++i) // Associate textures with sprites.
        spr_[i].setTexture(tex_[i]);
}

/**
 * Brief: Overloaded draw method inherited from sf::Drawable. Called by
 *        a sf::RenderTarget instance in the sf::RenderTarget::draw() method.
 * Param: A reference to the sf::RenderTarget instance that draws this
 *        object.
 * Param: States that indicate the transformation.
 */
void Tank::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(name_text_, states);
    target.draw(score_text_, states);
    states.transform *= getTransform(); // Modify the transformation.
    target.draw(spr_[dir_], states);
}

/**
 * Brief: Changes the direction that is used when drawing the sprite.
 * Param: The direction represented by the dir enum from Protocol.hpp.
 */
void Tank::set_dir(DIRECTION::dir d)
{
    dir_ = d;
}

/**
 * Brief: Increments the score and updates the score text.
 */
void Tank::add_point()
{
    ++score_;
    score_text_.setString(std::to_string(score_));
}

/**
 * Brief: Sets the name of the player associated with this tank.
 */
void Tank::set_name(std::string n)
{
    name_ = n;
    name_text_.setString(name_);
}

/**
 * Brief: Sets the movement status.
 * Param: Bool indicating the new movement status.
 */
void Tank::set_moving(bool mov)
{
    moving_ = mov;
}

/**
 * Brief: Updates the bounding rectangle and returns a reference to it.
 */
sf::FloatRect Tank::get_bounds()
{
    sf::FloatRect tmp = spr_[dir_].getLocalBounds();

    tmp.width += 5;
    tmp.height += 5;
    tmp.left = getPosition().x - tmp.width / 2;
    tmp.top = getPosition().y - tmp.height / 2;
    return tmp;
}

/**
 * Brief: Returns the id associated with this tank.
 * Note: Probably not needed, index of this tank in an array should be the id.
 */
int Tank::get_id()
{
    return id_;
}

/**
 * Brief: Returns the score of the player using this tank.
 */
int Tank::get_score()
{
    return score_;
}

/**
 * Brief: Returns the movement status of this tank.
 */
bool Tank::is_moving()
{
    return moving_;
}

/**
 * Brief: Returns the name of the player associated with this tank.
 */
std::string Tank::get_name()
{
    return name_;
}

/**
 * Brief: Returns the direction of this tank.
 */
DIRECTION::dir Tank::get_dir()
{
    return dir_;
}
