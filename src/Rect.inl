template<class T>
Rect<T>::Rect() noexcept:
    x(0), y(0), width(0), height(0)
{
}

template<class T>
Rect<T>::Rect(T X, T Y, T W, T H) noexcept:
    x(static_cast<T>(X)), 
    y(static_cast<T>(Y)), 
    width(static_cast<T>(W)), 
    height(static_cast<T>(H))
{
}

template<class T>
Rect<T>::Rect(const Rect& other) noexcept:
    Rect(other.x, other.y, other.width, other.height)
{
}

template<class T>
T Rect<T>::getLeft() const noexcept
{
    if constexpr (std::is_same<T, float>::value)   
        return x - (width * 0.5f);

    else if constexpr (std::is_same<T, unsigned>::value)
        return x - (width >> 1);

    else if constexpr (std::is_same<T, int>::value)
        return x - (std::abs(width) >> 1);

    return static_cast<T>(0);
}

template<class T>
T Rect<T>::getTop() const noexcept
{
    if constexpr (std::is_same<T, float>::value)
        return y + (height * 0.5f);

    else if constexpr (std::is_same<T, unsigned>::value)
        return y + (height >> 1);

    else if constexpr (std::is_same<T, int>::value)
        return y + (std::abs(height) >> 1);

    return static_cast<T>(0);
}

template<class T>
T Rect<T>::getRight() const noexcept
{
    if constexpr (std::is_same<T, float>::value)
        return x + (width * 0.5f);

    else if constexpr (std::is_same<T, unsigned>::value)
        return x + (width >> 1);

    else if constexpr (std::is_same<T, int>::value)
        return x + (std::abs(width) >> 1);

    return static_cast<T>(0);
}

template<class T>
T Rect<T>::getBottom() const noexcept
{
    if constexpr (std::is_same<T, float>::value)
        return y - (height * 0.5f);

    else if constexpr (std::is_same<T, unsigned>::value)
        return y - (height >> 1);

    else if constexpr (std::is_same<T, int>::value)
        return y - (std::abs(height) >> 1);

    return static_cast<T>(0);
}

template<class T>
bool Rect<T>::contains(const Rect<T>& other) const noexcept
{
    if ((x > other.x)                       || 
        (y > other.y)                       ||
        (x + width < other.x + other.width) ||
        (y + height < other.y + other.height)) 
            return false;

    return true; // within bounds
}

template<class T>
bool Rect<T>::intersects(const Rect<T>& other) const noexcept
{
    if ((x > other.x + other.width)  ||
        (x + width < other.x)        ||
        (y > other.y + other.height) ||
        (y + height < other.y))
            return false;

    return true; // intersection
}