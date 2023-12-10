#ifndef COLLIDABLE_HPP
#define COLLIDABLE_HPP

#include "Rect.hpp"

struct Collidable
{
    friend class QuadTree;

public:
    Collidable() noexcept;
    Collidable(const Rect<float>& bounds, void* data) noexcept;

    Collidable(const Collidable&)  = delete;
    Collidable(const Collidable&&) = delete;
    Collidable& operator = (const Collidable&)  = delete;
    Collidable& operator = (const Collidable&&) = delete;

    Rect<float> m_bounds;
    void*       m_data; // Replace with the desired component

private:
    class QuadTree* quadTree;   
};

#endif // !COLLIDABLE_HPP