#include "Collidable.hpp"

Collidable::Collidable() noexcept :
    m_bounds(), m_data(nullptr)
{
}

Collidable::Collidable(const Rect<float>& bounds, void* data) noexcept:
    m_bounds(bounds), m_data(data)
{
}