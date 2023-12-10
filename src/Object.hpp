#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Collidable.hpp"

extern Rect<float> MAP_BOUNDS;

class Object
{
public:
    Object(float x, float y, float width, float height) noexcept;

    void move() noexcept;

    sf::RectangleShape m_shape;
    Collidable         m_item;
    sf::Vector2f       m_velocity;
};

#endif // !OBJECT_HPP