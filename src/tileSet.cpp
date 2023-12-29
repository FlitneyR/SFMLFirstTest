#include <tileSet.hpp>
#include <csvParser.hpp>
#include <fstream>

TileSet::TileSet(
    const std::string& textureFilename,
    int tileSetColumns,
    int tileSetRows,
    int scale,
    const std::string& layoutFilename
)   :
    m_tileSetRows(tileSetRows),
    m_tileSetColumns(tileSetColumns),
    m_scale(scale)
{
    if (!m_texture.loadFromFile(textureFilename))
        throw std::runtime_error("Failed to open tileset texture: " + textureFilename);

    std::ifstream layoutFile(layoutFilename);
    CSVParser layout(layoutFile, false);
    
    m_gridColumns = layout.getColumnCount();
    m_gridRows = layout.getRowCount();

    m_cells.resize(m_gridRows * m_gridColumns);
    m_vertices.resize(6 * m_gridRows * m_gridColumns);
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    sf::Vector2u textureSize = m_texture.getSize();
    sf::Vector2i tileSize {
        static_cast<int>(textureSize.x / m_tileSetColumns),
        static_cast<int>(textureSize.y / m_tileSetRows)
    };

    sf::Vector2i gridSize = tileSize * m_scale;

    for (int j = 0; j < m_gridRows; j++)
    for (int i = 0; i < m_gridColumns; i++) {
        int tileIndex = std::atoi(layout.getCell(j, i).c_str());

        m_cells[i + j * m_gridColumns] = tileIndex;

        int tileColumn = tileIndex % m_tileSetColumns;
        int tileRow = tileIndex / m_tileSetColumns;

        sf::IntRect tile {
            { tileColumn * tileSize.x, tileRow * tileSize.y },
            tileSize
        };

        sf::IntRect cell {
            { i * gridSize.x, j * gridSize.y },
            gridSize
        };

        int firstVertex = 6 * (m_gridColumns * j + i);

        // top left
        m_vertices[firstVertex + 0].position.x = cell.left;
        m_vertices[firstVertex + 0].position.y = cell.top;
        m_vertices[firstVertex + 0].texCoords.x = tile.left;
        m_vertices[firstVertex + 0].texCoords.y = tile.top;

        // top right
        m_vertices[firstVertex + 1].position.x = cell.left + cell.width;
        m_vertices[firstVertex + 1].position.y = cell.top;
        m_vertices[firstVertex + 1].texCoords.x = tile.left + tile.width;
        m_vertices[firstVertex + 1].texCoords.y = tile.top;

        // bottom left
        m_vertices[firstVertex + 2].position.x = cell.left;
        m_vertices[firstVertex + 2].position.y = cell.top + cell.height;
        m_vertices[firstVertex + 2].texCoords.x = tile.left;
        m_vertices[firstVertex + 2].texCoords.y = tile.top + tile.height;

        // bottom left
        m_vertices[firstVertex + 3].position.x = cell.left;
        m_vertices[firstVertex + 3].position.y = cell.top + cell.height;
        m_vertices[firstVertex + 3].texCoords.x = tile.left;
        m_vertices[firstVertex + 3].texCoords.y = tile.top + tile.height;

        // top right
        m_vertices[firstVertex + 4].position.x = cell.left + cell.width;
        m_vertices[firstVertex + 4].position.y = cell.top;
        m_vertices[firstVertex + 4].texCoords.x = tile.left + tile.width;
        m_vertices[firstVertex + 4].texCoords.y = tile.top;

        // bottom right
        m_vertices[firstVertex + 5].position.x = cell.left + cell.width;
        m_vertices[firstVertex + 5].position.y = cell.top + cell.height;
        m_vertices[firstVertex + 5].texCoords.x = tile.left + tile.width;
        m_vertices[firstVertex + 5].texCoords.y = tile.top + tile.height;
    }
}

int TileSet::getCellType(const sf::Vector2i& cell) {
    return m_cells[cell.x + cell.y * m_gridColumns];
}

sf::FloatRect TileSet::getCellBounds(const sf::Vector2i& cell) {
    sf::Vector2u textureSize = m_texture.getSize();
    sf::Vector2i tileSize {
        static_cast<int>(textureSize.x / m_tileSetColumns),
        static_cast<int>(textureSize.y / m_tileSetRows)
    };

    sf::Vector2i gridSize = tileSize * m_scale;

    return {
        {
            static_cast<float>(gridSize.x * cell.x),
            static_cast<float>(gridSize.y * cell.y),
        },
        {
            static_cast<float>(gridSize.x),
            static_cast<float>(gridSize.y)
        }
    };
}

sf::Vector2i TileSet::getCellAtPosition(sf::Vector2f position) {
    sf::Vector2u textureSize = m_texture.getSize();
    sf::Vector2i tileSize {
        static_cast<int>(textureSize.x / m_tileSetColumns),
        static_cast<int>(textureSize.y / m_tileSetRows)
    };

    sf::Vector2i gridSize = tileSize * m_scale;

    position.x /= gridSize.x;
    position.y /= gridSize.y;

    return {
        static_cast<int>(position.x),
        static_cast<int>(position.y)
    };
}

void TileSet::highlightCell(sf::RenderTarget& target, const sf::Vector2i& cell, const sf::Color& color) {
    sf::FloatRect cellBounds = getCellBounds(cell);

    sf::RectangleShape rect;

    rect.setPosition(cellBounds.getPosition());
    rect.setSize(cellBounds.getSize());
    rect.setFillColor(color);

    target.draw(rect);
}

void TileSet::draw(sf::RenderTarget& target) {
    auto states = sf::RenderStates::Default;
    states.texture = &m_texture;
    target.draw(m_vertices, states);
}
