#include <SFML/Graphics.hpp>
#include <tileSet.hpp>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, const char** argv) {
    if (argc != 6) {
        std::cout << "Incorrect argument list\n"
                  << "\tPlease provide:\n"
                  << "\t1 - Path to tile set image\n"
                  << "\t2 - Number of columns in the tileset image\n"
                  << "\t3 - Number of rows in the tileset image\n"
                  << "\t4 - The maps scale multiplier\n"
                  << "\t5 - Path to the map csv file"
                  << std::endl;
        return 1;
    }

    sf::RenderWindow window { { 1280u, 720u }, "Map Editor" };
    window.setFramerateLimit(144);

    sf::Clock clock;
    sf::Time lastFrameStart = clock.getElapsedTime();

    std::string tileSetPath(argv[1]);
    int tileSetColumns = std::atoi(argv[2]);
    int tileSetRows = std::atoi(argv[3]);
    float scale = std::atof(argv[4]);
    std::string mapLayoutPath(argv[5]);

    TileSet tileSet { tileSetPath, tileSetColumns, tileSetRows, scale * 0.5f, tileSetColumns, tileSetRows };

    sf::Vector2i cell;
    for (cell.x = 0; cell.x < tileSetColumns; cell.x++)
    for (cell.y = 0; cell.y < tileSetRows; cell.y++) {
        tileSet[cell] = cell.x + cell.y * tileSetColumns;
    }
    tileSet.updateVertices();

    TileSet map;

    std::fstream layoutFile(mapLayoutPath);
    if (layoutFile.good()) {
        map = TileSet { tileSetPath, tileSetColumns, tileSetRows, scale, mapLayoutPath };
    } else {
        int columns, rows;
        std::cout << "Please enter the number of columns and rows of the new map grid: " << std::flush;
        std::cin >> columns >> rows;

        map = TileSet { tileSetPath, tileSetColumns, tileSetRows, scale, columns, rows };
    }
    layoutFile.close();

    sf::View view({ 0.f, 0.f }, sf::Vector2f { window.getSize() });

    sf::Vector2i currentBrush { 0, 0 };

    while (window.isOpen()) {
        sf::Vector2f mousePosition { sf::Mouse::getPosition(window) };
        sf::Vector2i currentTile = tileSet.getCellAtPosition(mousePosition);

        mousePosition += view.getCenter() - view.getSize() * 0.5f;

        sf::Vector2i currentCell = map.getCellAtPosition(mousePosition);

        bool mouseOnPallet = tileSet.isOnTileSet(currentTile);
        bool mouseOnMap = !mouseOnPallet && map.isOnTileSet(currentCell);

        for (auto event = sf::Event {}; window.pollEvent(event);)
        switch (event.type) {
        case sf::Event::Closed: window.close(); break;
        case sf::Event::Resized:
            view.setSize(event.size.width, event.size.height);
            break;
        case sf::Event::KeyPressed:
            switch (event.key.scancode) {
            case sf::Keyboard::Scancode::S:
                map.saveToFile(mapLayoutPath);
                break;
            default: break;
            }
            break;
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left && mouseOnPallet)
                currentBrush = currentTile;
            break;
        default: break;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseOnMap) {
            map[currentCell] = tileSet[currentBrush];
            map.updateVertices();
        }

        sf::Time currentFrameStart = clock.getElapsedTime();
        sf::Time deltaTime = currentFrameStart - lastFrameStart;

        sf::Vector2f cameraMovement {
            static_cast<float>(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) -
            static_cast<float>(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)),
            static_cast<float>(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) -
            static_cast<float>(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up))
        };

        cameraMovement *= 500.f * deltaTime.asSeconds();
        view.move(cameraMovement);

        window.clear();
        window.setView(view);

        map.draw(window);

        if (mouseOnMap) map.highlightCell(window, currentCell, { 255, 0, 0, 50 });

        sf::View UIview = view;
        UIview.setCenter(view.getSize() * 0.5f);
        window.setView(UIview);
        tileSet.draw(window);

        if (mouseOnPallet) tileSet.highlightCell(window, currentTile, { 0, 255, 0, 50 });
        tileSet.highlightCell(window, currentBrush, { 0, 0, 255, 50 });

        window.display();

        lastFrameStart = currentFrameStart;
    }    
    
    return 0;
}
