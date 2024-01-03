#include <player.hpp>
#include <orc.hpp>
#include <tileSet.hpp>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <csvParser.hpp>

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

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Incorrect argument list\n"
                  << "\tPlease provide:\n"
                  << "\tPath to the level csv file"
                  << std::endl;
        return 1;
    }

    std::string levelFilePath { argv[1] };

    std::fstream levelFile { levelFilePath };

    std::string tileSetPath, mapFilePath;
    int tileSetColumns, tileSetRows;
    float mapScale;

    TileSet map;
    Player player;
    std::vector<Orc> orcs;

    if (levelFile.good()) {
        CSVParser csvParser { levelFile, false };
        levelFile.close();

        tileSetPath    =           csvParser.getCell(0, 0);
        tileSetColumns = std::atoi(csvParser.getCell(0, 1).c_str());
        tileSetRows    = std::atoi(csvParser.getCell(0, 2).c_str());
        mapScale       = std::atof(csvParser.getCell(0, 3).c_str());
        mapFilePath    =           csvParser.getCell(0, 4);

        for (int i = 2; i < csvParser.getRowCount(); i++) {
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
    } else {
        std::cout << "Enter the path to the map's tile set: " << std::flush;
        std::cin >> tileSetPath;

        std::cout << "Enter the number of columns and rows in the tile set: " << std::flush;
        std::cin >> tileSetColumns >> tileSetRows;

        std::cout << "Enter the scaling factor of the map: " << std::flush;
        std::cin >> mapScale;

        std::cout << "Enter the path to the map's csv file: " << std::flush;
        std::cin >> mapFilePath;
    }

    map = TileSet { tileSetPath, tileSetColumns, tileSetRows, mapScale, mapFilePath };

    sf::RenderWindow window { { 1280u, 720u }, "Level Editor" };
    window.setFramerateLimit(144);

    sf::Clock clock;
    sf::Time lastFrameStart = clock.getElapsedTime();

    ObjectType brushType = e_None;

    Player previewPlayer;
    Orc previewOrc;

    sf::View view { {}, sf::Vector2f { window.getSize() } };

    while (window.isOpen()) {
        sf::Vector2f mousePosition { sf::Mouse::getPosition(window) };
        mousePosition += view.getCenter() - view.getSize() * 0.5f;

        previewPlayer.m_position = mousePosition;
        previewOrc.m_position = mousePosition;

        for (sf::Event event; window.pollEvent(event);)
        switch (event.type) {
        case sf::Event::KeyPressed:
            switch (event.key.scancode) {
            case sf::Keyboard::Scancode::Space:
                switch (brushType) {
                case e_None   : brushType = e_Player ; break;
                case e_Player : brushType = e_Orc    ; break;
                case e_Orc    : brushType = e_None   ; break;
                default       : brushType = e_Player ; break;
                }

                break;
            case sf::Keyboard::Scancode::S: {
                std::ofstream file(levelFilePath, std::ios::trunc);

                file << tileSetPath << ','
                     << tileSetColumns << ','
                     << tileSetRows << ','
                     << mapScale << ','
                     << mapFilePath << std::endl;
                
                file << "PLAYER" << ','
                     << player.m_position.x << ','
                     << player.m_position.y << std::endl;

                for (auto& orc : orcs)
                file << "ORC" << ','
                     << orc.m_position.x << ','
                     << orc.m_position.y << std::endl;

                file.close();

                break;
            }
            default: break;
            }

            break;
        case sf::Event::MouseButtonPressed:
            switch (brushType) {
            case e_Player:
                player = previewPlayer;

                break;
            case e_Orc:
                orcs.push_back(previewOrc);

                break;
            default: break;
            }

            break;
        case sf::Event::Closed:
            window.close();

            break;
        default: break;
        }

        sf::Time frameStart = clock.getElapsedTime();
        float deltaTime = (frameStart - lastFrameStart).asSeconds();

        sf::Vector2f viewCenter = view.getCenter();
        static constexpr float viewMovementSpeed = 500.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) viewCenter.x += deltaTime * viewMovementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left )) viewCenter.x -= deltaTime * viewMovementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down )) viewCenter.y += deltaTime * viewMovementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   )) viewCenter.y -= deltaTime * viewMovementSpeed;
        view.setCenter(viewCenter);

        player.animationUpdate(0.f);
        for (auto& orc : orcs) orc.updateAnimation(0.f);

        previewPlayer.animationUpdate(0.f);
        previewOrc.updateAnimation(0.f);

        window.clear();

        window.setView(view);

        map.draw(window);
        player.draw(window);
        for (auto& orc : orcs) orc.draw(window);

        switch (brushType) {
        case e_Player : previewPlayer.draw(window); break;
        case e_Orc    :    previewOrc.draw(window); break;
        default: break;
        }

        window.display();

        lastFrameStart = frameStart;
    }
}
