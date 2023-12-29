#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <player.hpp>
#include <tileSet.hpp>

static const std::string BACKGROUND_MUSIC_PATH {
    "../assets/audio/Minifantasy_Dungeon_Music/Music/Goblins_Dance_(Battle).wav"
};

static const std::string MAP_TILESET_PATH {
    "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/"
    "Minifantasy_Dungeon_Assets/Tileset/Tileset.png"
};

static const std::string MAP_LAYOUT { "../assets/testScene.csv" };

int main() {
    auto window = sf::RenderWindow{ { 1280u, 720u }, "SFML Test" };
    window.setFramerateLimit(144);

    sf::Clock clock;
    sf::Time lastFrameStart = clock.getElapsedTime();

    SoundManager::get().playMusic(BACKGROUND_MUSIC_PATH);
    
    Player player;

    sf::Vector2u windowSize = window.getSize();

    player.m_position = {
        static_cast<float>(windowSize.x / 2),
        static_cast<float>(windowSize.y / 2)
    };

    TileSet map(MAP_TILESET_PATH, 23, 14, 5, MAP_LAYOUT);
    map.addWallTypes({
        119, 220, 142, 143, 257, 280, 258, 143,
        125, 148, 263, 286, 145, 168, 208, 231,
        162, 240, 263, 286, 258, 283
    });

    sf::View view(player.m_position, sf::Vector2f(window.getSize()));

    while (window.isOpen()) {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        switch (event.type) {
        case sf::Event::Closed: window.close(); break;
        case sf::Event::Resized: view.setSize(event.size.width, event.size.height); break;
        case sf::Event::KeyPressed:
            switch (event.key.scancode) {
            case sf::Keyboard::Scancode::Z:
                player.attack();
                break;
            default: break;
            }
        default: break;
        }

        sf::Time currentFrameStart = clock.getElapsedTime();
        sf::Time deltaTime = currentFrameStart - lastFrameStart;

        player.movementUpdate(deltaTime.asSeconds());
        player.animationUpdate(deltaTime.asSeconds());
        player.tileSetCollisionUpdate(map);

        window.clear();

        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewTarget = player.m_position + player.getMovement() * 250.f;
        viewCenter = viewCenter + (viewTarget - viewCenter) * deltaTime.asSeconds();
        view.setCenter(viewCenter);

        window.setView(view);

        map.draw(window);
        player.draw(window);

        window.display();

        SoundManager::get().cleanUpFinishedSounds();

        lastFrameStart = currentFrameStart;
    }
}
