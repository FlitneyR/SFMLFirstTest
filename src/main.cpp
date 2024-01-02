#include <iostream>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <player.hpp>
#include <tileSet.hpp>
#include <orc.hpp>

static const std::string BACKGROUND_MUSIC_PATH {
    "../assets/audio/Minifantasy_Dungeon_Music/Music/Goblins_Den_(Regular).wav"
};

static const std::string BATTLE_MUSIC_PATH {
    "../assets/audio/Minifantasy_Dungeon_Music/Music/Goblins_Dance_(Battle).wav"
};

static const std::string ORC_HIT_SOUND {
    "../assets/audio/Minifantasy_Dungeon_SFX/21_orc_damage_2.wav"
};

static const std::string MAP_TILESET_PATH {
    "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Tileset/Tileset.png"
};

static const std::string MAP_LAYOUT { "../assets/testScene.csv" };

float randomFloat(float low = -1.f, float high = 1.f) {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> dist(low, high);

    return dist(gen);
}

int main() {
    sf::RenderWindow window { { 1280u, 720u }, "SFML Test" };
    window.setFramerateLimit(144);

    sf::Music& backgroundMusic = SoundManager::get().playMusic(BACKGROUND_MUSIC_PATH);
    sf::Music& battleMusic = SoundManager::get().playMusic(BATTLE_MUSIC_PATH);
    sf::SoundBuffer& orcHitSound = SoundManager::get().loadSound(ORC_HIT_SOUND);

    battleMusic.stop();
    
    Player player;

    std::vector<Orc> orcs;

    player.m_position = { 500.f, 500.f };
    // orc.m_position = { 700.f, 500.f };

    TileSet map(MAP_TILESET_PATH, 23, 14, 5, MAP_LAYOUT);
    map.addWallTypes({
        119, 220, 142, 143, 257, 280, 258, 143,
        125, 148, 263, 286, 145, 168, 208, 231,
        162, 240, 263, 286, 258, 283,  54,  77
    });

    sf::FloatRect mapBounds = map.getBounds();

    sf::View view(player.m_position, sf::Vector2f(window.getSize()));

    sf::Clock clock;
    sf::Time lastFrameStart = clock.getElapsedTime();

    int hitCount = 0;

    float newOrcTimer = 0.f;

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

        if ((newOrcTimer -= deltaTime.asSeconds()) <= 0.f) {
            newOrcTimer = 3.f;

            orcs.emplace_back();
            orcs.back().m_position.x = randomFloat(mapBounds.left + 200.f, mapBounds.left + mapBounds.width - 200.f);
            orcs.back().m_position.y = randomFloat(mapBounds.top + 200.f, mapBounds.top + mapBounds.height - 200.f);
        }

        player.movementUpdate(deltaTime.asSeconds());
        player.tileSetCollisionUpdate(map);
        player.animationUpdate(deltaTime.asSeconds());

        for (auto it = orcs.begin(); it != orcs.end();) {
            auto& orc = *it;

            bool orcHasReachedPlayer = orc.runTowards(player.m_position);

            if (!orc.isAttacking())
                orc.movementUpdate(deltaTime.asSeconds());
            
            orc.tileSetCollisionUpdate(map);
            orc.updateAnimation(deltaTime.asSeconds());

            if (orcHasReachedPlayer && orc.canAttack())
                orc.attack();

            if (orc.canTakeDamage() && player.isSwordCollidingWith(orc.getBounds()))
                orc.takeDamage(5.f);

            if (!orc.isAlive()) {
                std::swap(*it, orcs.back());
                orcs.pop_back();
            } else it++;
        }

        window.clear();

        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewTarget = player.m_position + player.getMovement() * 250.f;
        viewCenter = viewCenter + (viewTarget - viewCenter) * deltaTime.asSeconds();
        view.setCenter(viewCenter);

        window.setView(view);

        map.draw(window);

        player.draw(window);
        
        for (auto& orc : orcs)
            orc.draw(window);

        window.display();

        SoundManager::get().cleanUpFinishedSounds();

        lastFrameStart = currentFrameStart;
    }
}
