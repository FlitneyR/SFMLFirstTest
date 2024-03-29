#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <player.hpp>
#include <tileSet.hpp>
#include <orc.hpp>
#include <csvParser.hpp>

static const std::string BACKGROUND_MUSIC_PATH {
    "../assets/audio/Minifantasy_Dungeon_Music/Music/Goblins_Den_(Regular).wav"
};

static const std::string BATTLE_MUSIC_PATH {
    "../assets/audio/Minifantasy_Dungeon_Music/Music/Goblins_Dance_(Battle).wav"
};

static const std::string ORC_HIT_SOUND {
    "../assets/audio/Minifantasy_Dungeon_SFX/21_orc_damage_2.wav"
};

static const std::string LEVEL_PATH { "../assets/testLevel.csv" };

enum ObjectType : int {
    e_None = 0,
    e_Player,
    e_Orc,
};

std::map<std::string, ObjectType> s_objectNameToEnum {
    { "PLAYER", e_Player },
    { "ORC", e_Orc }
};

ObjectType parseObject(const std::string& name) {
    auto it = s_objectNameToEnum.find(name);
    if (it == s_objectNameToEnum.end()) return e_None;
    else return it->second;
}

int main() {
    sf::RenderWindow window { { 1280u, 720u }, "SFML Test" };
    window.setFramerateLimit(144);

    sf::Music& backgroundMusic = SoundManager::get().playMusic(BACKGROUND_MUSIC_PATH);
    sf::Music& battleMusic = SoundManager::get().playMusic(BATTLE_MUSIC_PATH);
    battleMusic.stop();
    
    TileSet map;
    Player player;
    std::vector<Orc> orcs;

    std::ifstream levelFile(LEVEL_PATH);
    if (!levelFile.good())
        throw std::runtime_error("Could not open level file: " + LEVEL_PATH);
    
    CSVParser csvParser { levelFile, false };
    levelFile.close();

    map = TileSet {
                                     csvParser.getCell(0, 0),
                           std::atoi(csvParser.getCell(0, 1).c_str()),
                           std::atoi(csvParser.getCell(0, 2).c_str()),
        static_cast<float>(std::atof(csvParser.getCell(0, 3).c_str())),
                                     csvParser.getCell(0, 4)
    };

    for (int i = 1; i < csvParser.getRowCount(); i++) {
        switch (parseObject(csvParser.getCell(i, 0))) {
        case e_Player: {
            player.m_position.x = std::atof(csvParser.getCell(i, 1).c_str());
            player.m_position.y = std::atof(csvParser.getCell(i, 2).c_str());

            break;
        }
        case e_Orc: {
            orcs.emplace_back();
            orcs.back().m_position.x = std::atof(csvParser.getCell(i, 1).c_str());
            orcs.back().m_position.y = std::atof(csvParser.getCell(i, 2).c_str());

            break;
        }
        default:
            std::cerr << "Didn't recognise object name: "
                        << csvParser.getCell(i, 0)
                        << std::endl;
            break;
        }
    }

    sf::FloatRect mapBounds = map.getBounds();

    sf::View view(player.m_position, sf::Vector2f(window.getSize()));

    sf::Clock clock;
    sf::Time lastFrameStart = clock.getElapsedTime();

    int hitCount = 0;

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
        float deltaTime = (currentFrameStart - lastFrameStart).asSeconds();

        player.movementUpdate(deltaTime);
        player.tileSetCollisionUpdate(map);
        player.animationUpdate(deltaTime);

        for (auto it = orcs.begin(); it != orcs.end();) {
            auto& orc = *it;

            bool orcHasReachedPlayer = orc.runTowards(player.m_position);

            if (!orc.isAttacking())
                orc.movementUpdate(deltaTime);
            
            orc.tileSetCollisionUpdate(map);
            orc.updateAnimation(deltaTime);

            if (orcHasReachedPlayer && orc.canAttack())
                orc.attack();

            if (orc.canTakeDamage() && player.isSwordCollidingWith(orc.getBounds()))
                orc.takeDamage(5.f);

            if (!orc.isAlive()) {
                std::swap(*it, orcs.back());
                orcs.pop_back();
            } else it++;
        }

        Orc::preventIntersection(orcs, deltaTime);

        window.clear();

        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewTarget = player.m_position + player.getMovement() * 250.f;
        viewCenter = viewCenter + (viewTarget - viewCenter) * deltaTime;
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
