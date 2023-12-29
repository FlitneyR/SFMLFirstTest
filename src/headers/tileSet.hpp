#pragma once

#include <SFML/Graphics.hpp>
#include <set>

class TileSet {
    sf::Texture m_texture;
    sf::VertexArray m_vertices;

    std::vector<int> m_cells;
    std::set<int> m_wallTypes;

    int m_tileSetRows;
    int m_tileSetColumns;
    int m_gridRows;
    int m_gridColumns;
    int m_scale;

public:
    TileSet(const std::string& textureFilename,
            int tileSetColumns,
            int tileSetRows,
            int scale,
            const std::string& layoutFilename);

    void addWallTypes(const std::vector<int>& indices) { for (auto index : indices) addWallType(index); }
    void addWallType(int index) { m_wallTypes.insert(index); }

    bool isWallType(int type) {
        return m_wallTypes.find(type) != m_wallTypes.end();
    }

    int getCellType(const sf::Vector2i& cell);

    sf::FloatRect getCellBounds(const sf::Vector2i& cell);
    sf::Vector2i getCellAtPosition(sf::Vector2f position);

    void highlightCell(sf::RenderTarget& target, const sf::Vector2i& cell, const sf::Color& color);

    void draw(sf::RenderTarget& target);
};
