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
    float m_scale;

    void setVertex(int index, sf::Vector2f position, sf::Vector2f texCoord) {
        m_vertices[index].position = position;
        m_vertices[index].texCoords = texCoord;
    }

public:
    TileSet() = default;

    TileSet(const std::string& textureFilename,
            int tileSetColumns,
            int tileSetRows,
            float scale,
            const std::string& layoutFilename);
    
    TileSet(const std::string& textureFilename,
            int tileSetColumns,
            int tileSetRows,
            float scale,
            int gridColumns,
            int gridRows);

    void saveToFile(const std::string& layoutFilename);
    
    void updateVertices();
    bool isOnTileSet(const sf::Vector2i& cell) {
        return cell.x >= 0
            && cell.y >= 0
            && cell.x < m_gridColumns
            && cell.y < m_gridRows;
    }

    void addWallTypes(const std::vector<int>& indices) {
        for (auto index : indices) addWallType(index);
    }
    
    void addWallType(int index) { m_wallTypes.insert(index); }

    bool isWallType(int type) {
        return m_wallTypes.find(type) != m_wallTypes.end();
    }

    int& getCellType(const sf::Vector2i& cell);
    int& operator[](const sf::Vector2i& cell) {
        return getCellType(cell);
    }
    
    sf::FloatRect getBounds() const;

    sf::FloatRect getCellBounds(const sf::Vector2i& cell);
    sf::Vector2i getCellAtPosition(sf::Vector2f position);

    void highlightCell(sf::RenderTarget& target, const sf::Vector2i& cell, const sf::Color& color);

    void draw(sf::RenderTarget& target);
};
