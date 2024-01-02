#include <tileSet.hpp>
#include <csvParser.hpp>
#include <fstream>

TileSet::TileSet(
    const std::string& textureFilename,
    int tileSetColumns,
    int tileSetRows,
    float scale,
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
    for (int j = 0; j < m_gridRows; j++)
    for (int i = 0; i < m_gridColumns; i++)
        m_cells[i + j * m_gridColumns] = std::atoi(layout.getCell(j, i).c_str());
    
    updateVertices();
}

TileSet::TileSet(
    const std::string& textureFilename,
    int tileSetColumns,
    int tileSetRows,
    float scale,
    int gridColumns,
    int gridRows
)   :
    m_tileSetRows(tileSetRows),
    m_tileSetColumns(tileSetColumns),
    m_scale(scale),
    m_gridColumns(gridColumns),
    m_gridRows(gridRows),
    m_cells(gridRows * gridColumns, 0)
{
    if (!m_texture.loadFromFile(textureFilename))
        throw std::runtime_error("Failed to open tileset texture: " + textureFilename);
    
    updateVertices();
}

void TileSet::saveToFile(const std::string& layoutFilename) {
    std::ofstream file(layoutFilename);

    for (int row = 0; row < m_gridRows; row++)
    for (int column = 0; column < m_gridColumns; column++) {
        file << getCellType({ column, row });

        if (column + 1 < m_gridColumns) file << ", ";
        else file << '\n';
    }
}

void TileSet::updateVertices() {
    m_vertices.resize(6 * m_gridRows * m_gridColumns);
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);

    sf::Vector2u textureSize = m_texture.getSize();
    sf::Vector2f tileSize {
        textureSize.x / static_cast<float>(m_tileSetColumns),
        textureSize.y / static_cast<float>(m_tileSetRows)
    };

    sf::Vector2f gridSize = tileSize * m_scale;

    for (int j = 0; j < m_gridRows; j++)
    for (int i = 0; i < m_gridColumns; i++) {
        int tileIndex = m_cells[i + j * m_gridColumns];

        int tileColumn = tileIndex % m_tileSetColumns;
        int tileRow = tileIndex / m_tileSetColumns;

        sf::FloatRect tile { { tileColumn * tileSize.x, tileRow * tileSize.y }, tileSize };
        sf::FloatRect cell { { i * gridSize.x, j * gridSize.y }, gridSize };

        int vertexIndex = 6 * (m_gridColumns * j + i);
        
        sf::Vector2f cellTopLeft        { cell.left                 , cell.top              };
        sf::Vector2f cellTopRight       { cell.left + cell.width    , cell.top              };
        sf::Vector2f cellBottomLeft     { cell.left                 , cell.top + cell.width };
        sf::Vector2f cellBottomRight    { cell.left + cell.width    , cell.top + cell.width };
        
        sf::Vector2f tileTopLeft        { tile.left                 , tile.top              };
        sf::Vector2f tileTopRight       { tile.left + tile.width    , tile.top              };
        sf::Vector2f tileBottomLeft     { tile.left                 , tile.top + tile.width };
        sf::Vector2f tileBottomRight    { tile.left + tile.width    , tile.top + tile.width };

        setVertex(vertexIndex++, cellTopLeft, tileTopLeft);
        setVertex(vertexIndex++, cellTopRight, tileTopRight);
        setVertex(vertexIndex++, cellBottomLeft, tileBottomLeft);
        setVertex(vertexIndex++, cellBottomLeft, tileBottomLeft);
        setVertex(vertexIndex++, cellTopRight, tileTopRight);
        setVertex(vertexIndex++, cellBottomRight, tileBottomRight);
    }
}

int& TileSet::getCellType(const sf::Vector2i& cell) {
    return m_cells[cell.x + cell.y * m_gridColumns];
}

sf::FloatRect TileSet::getBounds() const {
    sf::Vector2u textureSize = m_texture.getSize();
    sf::Vector2f tileSize {
        textureSize.x / static_cast<float>(m_tileSetColumns),
        textureSize.y / static_cast<float>(m_tileSetRows)
    };

    sf::Vector2f gridSize = tileSize * m_scale;

    return sf::FloatRect {
        { 0, 0 },
        { gridSize.x * m_gridColumns, gridSize.y * m_gridRows }
    };
}

sf::FloatRect TileSet::getCellBounds(const sf::Vector2i& cell) {
    sf::Vector2u textureSize = m_texture.getSize();
    sf::Vector2f tileSize {
        textureSize.x / static_cast<float>(m_tileSetColumns),
        textureSize.y / static_cast<float>(m_tileSetRows)
    };

    sf::Vector2f gridSize = tileSize * m_scale;

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
    sf::Vector2f tileSize {
        textureSize.x / static_cast<float>(m_tileSetColumns),
        textureSize.y / static_cast<float>(m_tileSetRows)
    };

    sf::Vector2f gridSize = tileSize * m_scale;

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
