#include "Object.hpp"

Object::Object(float x, float y, float width, float height) noexcept:
    m_item(Rect<float>(x, y, width, height), static_cast<void*>(this))
{
    m_shape.setPosition(m_item.m_bounds.x, m_item.m_bounds.y);
    m_shape.setSize({ m_item.m_bounds.width, m_item.m_bounds.height });
    m_velocity.x = (rand() % 201 - 100) * 0.05f;
    m_velocity.y = (rand() % 201 - 100) * 0.05f;
}

void Object::move() noexcept
{
    if (m_item.m_bounds.x + m_velocity.x < 0 || m_item.m_bounds.x + m_item.m_bounds.width + m_velocity.x > MAP_BOUNDS.width)
        m_velocity.x = -m_velocity.x;

    if (m_item.m_bounds.y + m_velocity.y < 0 || m_item.m_bounds.y + m_item.m_bounds.height + m_velocity.y > MAP_BOUNDS.height)
        m_velocity.y = -m_velocity.y;

    m_item.m_bounds.x += m_velocity.x;
    m_item.m_bounds.y += m_velocity.y;
    m_shape.setPosition(m_item.m_bounds.x, m_item.m_bounds.y);
}