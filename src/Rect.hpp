#ifndef RECT_HPP
#define RECT_HPP

#include <cstdlib>
#include <type_traits>

template<class T>
class Rect
{
public:
    Rect()                   noexcept;
    Rect(T X, T Y, T W, T H) noexcept;
    Rect(const Rect& other)  noexcept;

    T getLeft()   const noexcept;
    T getTop()    const noexcept;
    T getRight()  const noexcept;
    T getBottom() const noexcept;

    bool contains(const Rect& other)   const noexcept;
    bool intersects(const Rect& other) const noexcept;

    T x;
    T y;
    T width;
    T height;
};

#include "Rect.inl"

#endif RECT_HPP // !RECT_HPP