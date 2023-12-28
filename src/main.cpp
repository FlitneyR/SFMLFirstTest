#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <player.hpp>
#include <tileSet.hpp>

int main() {
    auto window = sf::RenderWindow{ { 1600u, 800u }, "SFML Test" };
    window.setFramerateLimit(144);

    sf::Clock clock;
    sf::Time lastFrameStart = clock.getElapsedTime();

    SoundManager::get().playMusic("../assets/audio/Minifantasy_Dungeon_Music/Music/Goblins_Dance_(Battle).wav");
    
    Player player;

    sf::Vector2u windowSize = window.getSize();

    player.m_position = {
        static_cast<float>(windowSize.x / 2),
        static_cast<float>(windowSize.y / 2)
    };

    TileSet tileSet("../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Tileset/Tileset.png", 23, 14, 5, "../assets/testScene.csv");

    while (window.isOpen()) {
        for (auto event = sf::Event{}; window.pollEvent(event);) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.scancode) {
                case sf::Keyboard::Scancode::Z:
                    player.attack();
                    break;
                default: break;
                }
            default: break;
            }
        }

        sf::Time currentFrameStart = clock.getElapsedTime();
        sf::Time deltaTime = currentFrameStart - lastFrameStart;

        player.update(deltaTime.asSeconds());

        window.clear();

        tileSet.draw(window);
        player.draw(window);

        window.display();

        lastFrameStart = currentFrameStart;
    }
}
