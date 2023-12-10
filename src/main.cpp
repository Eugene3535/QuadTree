#include <ctime>
#include <sstream>

#include "QuadTree.hpp"
#include "Object.hpp"

Rect<float> MAP_BOUNDS = { 0, 0, 1280, 720 };

int main()
{
    sf::RenderWindow window(sf::VideoMode((unsigned)MAP_BOUNDS.width, (unsigned)MAP_BOUNDS.height), "QuadTree");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    QuadTree map = QuadTree(MAP_BOUNDS, 8, 4);
    std::vector<std::unique_ptr<Object>> objects;

    sf::Font font;
    font.loadFromFile("res/fonts/freshman.ttf");
    map.setFont(font);

    sf::Text info("", font);
    info.setCharacterSize(20);
    info.setFillColor(sf::Color::Black);
    info.setPosition(sf::Vector2f(4, 4));

    sf::RectangleShape textBox;
    textBox.setFillColor(sf::Color(204, 204, 204));

    sf::RectangleShape mouseBox;
    mouseBox.setOutlineThickness(3.0f);
    mouseBox.setFillColor(sf::Color(127, 0, 255, 0));
    mouseBox.setOutlineColor(sf::Color::Magenta);

    bool freezeObjects = false;
    Rect<float> mouseBoundary = { 0, 0, 20, 20 };

    while (window.isOpen())
    {
        sf::Event event;

        // Update controls
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
//   Esc = exit
                case sf::Keyboard::Escape:
                    window.close();
                    break;

//  F = freeze all objects
                case sf::Keyboard::F:
                    freezeObjects = !freezeObjects;
                    break;

//  C = clear quadtree and remove all objects
                case sf::Keyboard::C:
                    map.clear();
                    objects.clear();
                    break;

//  Up = increase size of mouse box
                case sf::Keyboard::Up:
                    mouseBoundary.width += 2;
                    mouseBoundary.height += 2;
                    break;

//  Down = decrease size of mouse box
                case sf::Keyboard::Down:
                    mouseBoundary.width -= 2;
                    mouseBoundary.height -= 2;
                    break;
                }
            }
        }

        window.clear();
        window.draw(map);

        // Update collisions
        std::vector<Object*> mouseCollisions;
        std::size_t collisions = 0;
        std::size_t qtCollisionChecks = 0;
        std::size_t bfCollisionChecks = 0;

        for (auto& obj : objects)
        {
            obj->m_shape.setFillColor(sf::Color::Blue);

            if (mouseBoundary.intersects(obj->m_item.m_bounds))
            {
                obj->m_shape.setFillColor(sf::Color::Red);
                mouseCollisions.push_back(obj.get());
                ++collisions;
            }

            bfCollisionChecks += objects.size();

            for (auto& found : map.getObjectsInBoundUnchecked(obj->m_item.m_bounds))
            {
                ++qtCollisionChecks;

                if (&obj->m_item != found && found->m_bounds.intersects(obj->m_item.m_bounds))
                {
                    ++collisions;
                    obj->m_shape.setFillColor(sf::Color::Red);
                }
            }

            if (!freezeObjects)
            {
                obj->move();
                map.update(&obj->m_item);
            }

            window.draw(obj->m_shape);
        }

        // Update mouse box
        mouseBoundary.x = sf::Mouse::getPosition(window).x;
        mouseBoundary.y = sf::Mouse::getPosition(window).y;
        mouseBox.setSize(sf::Vector2f((float)mouseBoundary.width, (float)mouseBoundary.height));
        mouseBox.setPosition((float)mouseBoundary.x, (float)mouseBoundary.y);

        // Left click = add objects
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && MAP_BOUNDS.contains(mouseBoundary))
        {
            objects.push_back(std::make_unique<Object>(mouseBoundary.getRight(), mouseBoundary.getTop(), rand() % 20 + 4, rand() % 20 + 4));
            map.insert(&objects.back()->m_item);
        }

        auto quick_remove = [](std::vector<std::unique_ptr<Object>>& v, std::size_t i)
        {
            v[i] = std::move(v.back());
            v.pop_back();
        };

        // Right click = remove objects within mouse box
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            for (auto obj : mouseCollisions)
            {
                map.remove(&obj->m_item);

                for (std::size_t i = 0; i < objects.size(); ++i)
                {
                    if (obj == objects[i].get()) 
                    {
                        quick_remove(objects, i);
                    }
                }
            }
        }

        // Display quadtree debug info
        std::stringstream ss;
        ss << "Total Children: " << map.totalChildren()
            << "\nTotal Objects: " << map.totalObjects()
            << "\nTotal Collisions: " << collisions
            << "\nQuadTree collision checks: " << qtCollisionChecks
            << "\nBrute force collision checks: " << bfCollisionChecks
            << "\nCollisions with mouse: " << mouseCollisions.size()
            << "\nObjects in this quad: " << map.getLeaf(mouseBoundary)->totalObjects();
        info.setString(ss.str());
        textBox.setSize(sf::Vector2f(info.getLocalBounds().width + 16, info.getLocalBounds().height + 16));
        window.draw(textBox);
        window.draw(info);
        window.draw(mouseBox);
        window.display();
    }
    
//  Cleanup all
    map.clear();
    objects.clear();

    return 0;
}