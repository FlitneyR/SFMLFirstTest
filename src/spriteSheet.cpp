#include <spriteSheet.hpp>

SpriteSheet::SpriteSheet(sf::Texture& texture, int rows, int columns, sf::FloatRect region) :
    m_rows(rows), m_columns(columns), m_index(0), m_animationRegion(region), r_texture(&texture)
{
    m_sprite.setTexture(getTexture());
}

bool SpriteSheet::hasFinished() const {
    return !m_loop && getIndex() >= m_rows * m_columns;
}

int SpriteSheet::getIndex() const {
    return std::floor(m_index);
}

void SpriteSheet::setIndex(float index) {
    float maxIndex = m_rows * m_columns;
    
    m_index = m_loop ? std::fmod(index, maxIndex)
                     : std::min(index, maxIndex);
    
    m_sprite.setTextureRect(getCurrentSpriteRect());

    m_sprite.setOrigin(getTileSize() * 0.5f);
}

void SpriteSheet::incrementIndex(float deltaTime) {
    setIndex(m_index + deltaTime * m_animationSpeed);
}

void SpriteSheet::draw(sf::RenderTarget& target) {
    target.draw(m_sprite);
}

sf::Vector2f SpriteSheet::getTileSize() {
    sf::Vector2u textureSize = getTexture().getSize();
    return {
        textureSize.x * m_animationRegion.width / m_columns,
        textureSize.y * m_animationRegion.height / m_rows,
    };
}

sf::IntRect SpriteSheet::getCurrentSpriteRect() {
    sf::Vector2u textureSize = getTexture().getSize();

    sf::Vector2i tileSize {
        static_cast<int>(m_animationRegion.width * textureSize.x / m_columns),
        static_cast<int>(m_animationRegion.height * textureSize.y / m_rows)
    };

    sf::Vector2i offset {
        static_cast<int>(m_animationRegion.left * textureSize.x),
        static_cast<int>(m_animationRegion.top * textureSize.y),
    };

    int index = hasFinished() ? getIndex() - 1 : getIndex();

    int currentRow = index / m_columns;
    int currentColumn = index % m_columns;

    return {
        {
            offset.x + tileSize.x * currentColumn,
            offset.y + tileSize.y * currentRow
        }, tileSize };
}
