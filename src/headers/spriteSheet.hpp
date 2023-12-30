#pragma once

#include <SFML/Graphics.hpp>

#include <string>

class SpriteSheet {
    sf::Texture* r_texture;

    sf::IntRect getCurrentSpriteRect();
    sf::Vector2f getTileSize();

    float m_index;

public:
    sf::Sprite m_sprite;
    sf::FloatRect m_animationRegion;

    int m_rows;
    int m_columns;
    bool m_loop = true;
    float m_animationSpeed = 15.f;

    SpriteSheet() = default;
    SpriteSheet(sf::Texture& texture, int rows, int columns, sf::FloatRect region = { { 0, 0 }, { 1, 1 } });

    sf::Texture& getTexture() { return *r_texture; }

    bool hasFinished();
    int getIndex();
    void setIndex(float index);
    void incrementIndex(float deltaTime);
    void draw(sf::RenderTarget& target);
};
