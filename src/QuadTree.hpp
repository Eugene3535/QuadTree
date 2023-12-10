#ifndef QUAD_TREE_HPP
#define QUAD_TREE_HPP

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Collidable.hpp"

class QuadTree:
    public sf::Drawable
{
public:
    QuadTree() noexcept;
    QuadTree(const Rect<float>& bounds, std::size_t capacity, std::size_t maxLevel) noexcept;
    QuadTree(const QuadTree& qt) noexcept;
    ~QuadTree();

    bool insert(Collidable* obj) noexcept;
    bool remove(Collidable* obj) noexcept;
    bool update(Collidable* obj) noexcept;
    void clear() noexcept;

    std::vector<Collidable*>& getObjectsInBoundUnchecked(const Rect<float>& bounds) noexcept;
    std::size_t totalChildren() const noexcept;
    std::size_t totalObjects()  const noexcept;

    void setFont(const sf::Font& font) const noexcept;

    QuadTree* getLeaf(const Rect<float>& bounds) noexcept;

private:
    void subdivide() noexcept;
    void discardEmptyBuckets() noexcept;
    QuadTree* getChild(const Rect<float>& bounds) const noexcept;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    mutable sf::RectangleShape m_shape;
    mutable sf::Text           m_text;

    std::vector<Collidable*> m_objects;
    std::vector<Collidable*> m_foundedObjects;

    Rect<float> m_bounds;
    QuadTree*   m_parent;
    std::unique_ptr<QuadTree> m_children[4];

    std::size_t m_level;
    std::size_t m_capacity;
    std::size_t m_maxLevel;
    bool        m_isLeaf;
};

#endif // !QUAD_TREE_HPP