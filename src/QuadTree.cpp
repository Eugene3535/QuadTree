#include <sstream>
#include <algorithm>

#include "QuadTree.hpp"

QuadTree::QuadTree() noexcept:
    m_bounds(),
    m_parent(nullptr),
    m_level(0),
    m_capacity(0),
    m_maxLevel(0),
    m_isLeaf(true)
{
}

QuadTree::QuadTree(const Rect<float>& bounds, std::size_t capacity, std::size_t maxLevel) noexcept:
    QuadTree()
{
    m_bounds   = bounds;
    m_capacity = capacity;
    m_maxLevel = maxLevel;

    m_objects.reserve(capacity);
    m_foundedObjects.reserve(capacity);

    m_shape.setOutlineThickness(1.f);
    m_shape.setSize({ bounds.width - m_shape.getOutlineThickness(), bounds.height });
    m_shape.setOutlineColor(sf::Color::Black);
    m_shape.setPosition(bounds.x, bounds.y);
    m_shape.setFillColor(sf::Color(242, 242, 242));
    m_text.setFillColor(sf::Color(128, 128, 128));
}

QuadTree::QuadTree(const QuadTree& other) noexcept: 
    QuadTree(other.m_bounds, other.m_capacity, other.m_maxLevel)
{ 
}

QuadTree::~QuadTree()
{
    clear();
}

// Inserts an object into this quadtree
bool QuadTree::insert(Collidable* obj) noexcept
{
    if (!obj->quadTree)
    {
        if (!m_isLeaf)
        {
            // insert object into leaf
            if (QuadTree* child = getChild(obj->m_bounds))
                return child->insert(obj);
        }
        m_objects.push_back(obj);
        obj->quadTree = this;

        // Subdivide if required
        if (m_isLeaf && m_level < m_maxLevel && m_objects.size() >= m_capacity)
        {
            subdivide();
            update(obj);
        }

        return true;
    }

    return false;
}

// Removes an object from this quadtree
bool QuadTree::remove(Collidable* obj) noexcept
{
    if (!obj->quadTree) 
        return false; // Cannot exist in vector

    if (obj->quadTree != this) 
        return obj->quadTree->remove(obj);

    m_objects.erase(std::find(m_objects.begin(), m_objects.end(), obj));
    obj->quadTree = nullptr;
    discardEmptyBuckets();

    return true;
}

// Removes and re-inserts object into quadtree (for objects that move)
bool QuadTree::update(Collidable* obj) noexcept
{
    if (!remove(obj)) 
        return false;

    // Not contained in this node -- insert into parent
    if (m_parent != nullptr && !m_bounds.contains(obj->m_bounds))
        return m_parent->insert(obj);

    if (!m_isLeaf) 
    {
        // Still within current node -- insert into leaf
        if (QuadTree* child = getChild(obj->m_bounds))
            return child->insert(obj);
    }

    return insert(obj);
}

// Removes all objects and children from this quadtree
void QuadTree::clear() noexcept
{
    if (!m_objects.empty())
    {
        for (auto&& obj : m_objects)
            obj->quadTree = nullptr;

        m_objects.clear();
    }

    if (!m_isLeaf)
    {
        for (auto& child : m_children)
            child->clear();

        m_isLeaf = true;
    }
}

// Searches quadtree for objects within the provided boundary and returns them in vector
std::vector<Collidable*>& QuadTree::getObjectsInBoundUnchecked(const Rect<float>& bounds) noexcept
{
    m_foundedObjects.clear();

    for (const auto& obj : m_objects)
    {
        // Only check for intersection with OTHER boundaries
        m_foundedObjects.push_back(obj);
    }

    if (!m_isLeaf) 
    {
        // Get objects from leaves
        if (QuadTree* child = getChild(bounds)) 
        {
            child->getObjectsInBoundUnchecked(bounds);
            m_foundedObjects.insert(m_foundedObjects.end(), child->m_foundedObjects.begin(), child->m_foundedObjects.end());
        }
        else
            for (auto& leaf : m_children)
            {
                if (leaf->m_bounds.intersects(bounds)) 
                {
                    leaf->getObjectsInBoundUnchecked(bounds);
                    m_foundedObjects.insert(m_foundedObjects.end(), leaf->m_foundedObjects.begin(), leaf->m_foundedObjects.end());
                }
            }
    }

    return m_foundedObjects;
}

// Returns total children count for this quadtree
std::size_t QuadTree::totalChildren() const noexcept
{
    if (m_isLeaf)
        return 0;

    std::size_t total = 0;

    for (auto& child : m_children)
        total += child->totalChildren();

    return 4 + total;
}

// Returns total object count for this quadtree
std::size_t QuadTree::totalObjects() const noexcept
{
    std::size_t total = m_objects.size();

    if (!m_isLeaf) 
    {
        for (auto& child : m_children)
            total += child->totalObjects();
    }

    return total;
}

void QuadTree::setFont(const sf::Font& font) const noexcept
{
    m_text.setFont(font);
    m_text.setCharacterSize(40 / (m_level ? m_level : 1));
    m_text.setPosition(m_bounds.getRight() - (m_text.getLocalBounds().width * 0.5f), m_bounds.getTop() - 1 - (m_text.getLocalBounds().height * 0.5f));

    if (m_isLeaf) 
        return;

    for (auto& child : m_children)
        child->setFont(font);
}

// Subdivides into four quadrants
void QuadTree::subdivide() noexcept
{
    float x = 0.0f;
    float y = 0.0f;
    float width  = m_bounds.width * 0.5f;
    float height = m_bounds.height * 0.5f;

    for (unsigned i = 0; i < 4; ++i) 
    {
        switch (i) 
        {
            case 0: x = m_bounds.x + width; y = m_bounds.y;          break; // Top right
            case 1: x = m_bounds.x;         y = m_bounds.y;          break; // Top left
            case 2: x = m_bounds.x;         y = m_bounds.y + height; break; // Bottom left
            case 3: x = m_bounds.x + width; y = m_bounds.y + height; break; // Bottom right
        }

        m_children[i] = std::make_unique<QuadTree>(Rect<float>(x, y, width, height), m_capacity, m_maxLevel);
        m_children[i]->m_level = m_level + 1;
        m_children[i]->m_parent = this;
    }

    m_isLeaf = false;
}

// Discards buckets if all children are leaves and contain no objects
void QuadTree::discardEmptyBuckets() noexcept
{
    if (!m_objects.empty()) 
        return;

    if (!m_isLeaf) 
    {
        for (auto& child : m_children)
            if (!child->m_isLeaf || !child->m_objects.empty())
                return;
    }

    if (clear(), m_parent != nullptr)
        m_parent->discardEmptyBuckets();
}

QuadTree* QuadTree::getLeaf(const Rect<float>& bounds) noexcept
{
    QuadTree* leaf = this;

    if (!m_isLeaf) 
    {
        if (QuadTree* child = getChild(bounds))
            leaf = child->getLeaf(bounds);
    }

    return leaf;
}

// Returns child that contains the provided boundary
QuadTree* QuadTree::getChild(const Rect<float>& bounds) const noexcept 
{
    bool left  = bounds.x + bounds.width < m_bounds.getRight();
    bool right = bounds.x > m_bounds.getRight();

    if (bounds.y + bounds.height < m_bounds.getTop())
    {
        if (left)  return m_children[1].get(); // Top left
        if (right) return m_children[0].get(); // Top right
    }
    else if (bounds.y > m_bounds.getTop())
    {
        if (left)  return m_children[2].get(); // Bottom left
        if (right) return m_children[3].get(); // Bottom right
    }

    return nullptr; // Cannot contain boundary -- too large
}

void QuadTree::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    setFont(*m_text.getFont());

    if (!m_objects.empty())
        m_shape.setFillColor(sf::Color::White);
    else
        m_shape.setFillColor(sf::Color(242, 242, 242));

    target.draw(m_shape);

    if (!m_isLeaf) 
    {
        for (auto& child : m_children)
            target.draw(*child);
    }
    else 
    {
        std::stringstream ss;
        ss << m_level;
        m_text.setString(ss.str());
        target.draw(m_text);
    }
}