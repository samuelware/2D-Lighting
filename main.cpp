#include <SFML/Graphics.hpp>

class LightSystem : public sf::Drawable
{
public:
    LightSystem(const sf::Color& ambient_colour) : ambient_colour(ambient_colour)
    {
        light_texture.loadFromFile("Light.png");
        light_texture.setSmooth(true);
        circle.setTexture(&light_texture);
        texture.create(640, 480);
    }

    void setAmbientColour(const sf::Color& colour)
    {
        ambient_colour = colour;
    }

    uint16_t add(const sf::Vector2f& position, const sf::Color& colour, const float radius)
    {
        lights.emplace_back(++ids, position, colour, radius);
        return ids;
    }

    void remove(const uint16_t id)
    {
        auto it = std::find(lights.begin(), lights.end(), id);
        if (it == lights.end()) return;
        lights.erase(it);
    }

    void setPosition(const uint16_t id, const sf::Vector2f& position)
    {
        auto it = std::find(lights.begin(), lights.end(), id);
        if (it == lights.end()) return;
        it->position = position;
    }

    void setColour(const uint16_t id, const sf::Color& colour)
    {
        auto it = std::find(lights.begin(), lights.end(), id);
        if (it == lights.end()) return;
        it->colour = colour;
    }

    void setRadius(const uint16_t id, const float radius)
    {
        auto it = std::find(lights.begin(), lights.end(), id);
        if (it == lights.end()) return;
        it->radius = radius;
    }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        auto view = target.getView();
        rect.left = view.getCenter().x - (view.getSize().x / 2.0f);
        rect.height = view.getCenter().y - (view.getSize().y / 2.0f);
        rect.width = view.getSize().x;
        rect.height = view.getSize().y;
        texture.clear(ambient_colour);
        for (auto& light : lights)
        {
            circle.setRadius(light.radius);
            circle.setOrigin(light.radius, light.radius);
            circle.setPosition(light.position);
            if (rect.intersects(circle.getGlobalBounds()))
            {
                circle.setPosition(static_cast<sf::Vector2f>(texture.mapCoordsToPixel(light.position, view)));
                circle.setFillColor(light.colour);
                texture.draw(circle);
            }
        }
        texture.display();
        sprite.setTexture(texture.getTexture());
        target.setView(target.getDefaultView());
        target.draw(sprite, sf::BlendMultiply);
        target.setView(view);
    }

private:
    struct Light
    {
        Light(const uint16_t uid, const sf::Vector2f& position, const sf::Color& colour, const float radius)
            : uid(uid), position(position), colour(colour), radius(radius) {}

        bool operator==(const uint16_t rhs) {
            return uid == rhs;
        }

        uint16_t uid;
        sf::Vector2f position;
        sf::Color colour;
        float radius;
    };

private:
    uint16_t ids;
    std::vector<Light> lights;
    sf::Color ambient_colour;
    sf::Texture light_texture;
    mutable sf::FloatRect rect;
    mutable sf::RenderTexture texture;
    mutable sf::Sprite sprite;
    mutable sf::CircleShape circle;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "2D Lighting");
    window.setFramerateLimit(60);
    sf::Clock clock;
    sf::Time delta;

    sf::Texture world_texture;
    world_texture.loadFromFile("World.png");
    sf::Sprite world(world_texture);

    sf::Texture player_texture;
    player_texture.loadFromFile("Player.png");
    sf::Sprite player(player_texture);
    player.setOrigin(static_cast<sf::Vector2f>(player_texture.getSize()) / 2.0f);
    sf::Vector2f movement;
    float speed = 250;

    LightSystem lights(sf::Color(60, 60, 60));
    auto light = lights.add(player.getPosition(), sf::Color::Yellow, 75.0f);

    sf::View view;
    view.setSize(static_cast<sf::Vector2f>(window.getSize()));

    sf::Event event;
    while (window.isOpen())
    {
        delta = clock.restart();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Space:
                    lights.setColour(light, sf::Color::Green);
                    lights.setRadius(light, 250.0f);
                    break;
                case sf::Keyboard::A:
                    movement.x = -speed;
                    break;
                case sf::Keyboard::D:
                    movement.x = speed;
                    break;
                case sf::Keyboard::W:
                    movement.y = -speed;
                    break;
                case sf::Keyboard::S:
                    movement.y = speed;
                    break;
                }
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::A:
                case sf::Keyboard::D:
                    movement.x = 0.0f;
                    break;
                case sf::Keyboard::W:
                case sf::Keyboard::S:
                    movement.y = 0.0;
                    break;
                }
            }
        }

        player.move(movement * delta.asSeconds());
        lights.setPosition(light, player.getPosition());
        view.setCenter(player.getPosition());
        window.setView(view);

        window.clear();
        window.draw(world);
        window.draw(player);
        window.draw(lights);
        window.display();
    }

    return 0;
}