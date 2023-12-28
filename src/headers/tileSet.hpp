#pragma once

#include <SFML/Graphics.hpp>

class TileSet {
    sf::Texture m_texture;
    sf::VertexArray m_vertices;

    int m_tileSetRows;
    int m_tileSetColumns;
    int m_gridRows;
    int m_gridColumns;

public:
    TileSet(const std::string& textureFilename,
            int tileSetColumns,
            int tileSetRows,
            int scale,
            const std::string& layoutFilename);

    void draw(sf::RenderTarget& target);
};
