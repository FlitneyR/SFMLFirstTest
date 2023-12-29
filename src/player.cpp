#include <player.hpp>
#include <soundManager.hpp>
#include <iostream>

Player::Player() :
    m_attackSpriteSheet(
        s_attackSpriteSheetPath,
        1, 4,
        { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_walkSpriteSheet(
        s_walkSpriteSheetPath,
        1, 4,
        { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_idleSpriteSheet(
        s_idleSpriteSheetPath,
        1, 16,
        { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    )
{
    auto& soundManager = SoundManager::get();
    r_attackSound = &soundManager.loadSound(s_attackSoundPath);
    r_stepSound = &soundManager.loadSound(s_walkSoundPath);

    m_attackSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_attackSpriteSheet.m_loop = false;

    m_idleSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_idleSpriteSheet.m_animationSpeed = 5.f;

    m_walkSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_walkSpriteSheet.m_animationSpeed = 5.f;
}

sf::FloatRect Player::getBounds() {
    return {
        { m_position.x - 15.f, m_position.y - 15.f },
        { 30.f, 30.f }
    };
}

SpriteSheet& Player::getCurrentSpriteSheet() {
    return m_attacking  ? m_attackSpriteSheet :
           m_moving     ? m_walkSpriteSheet   :
                          m_idleSpriteSheet   ;
}

void Player::draw(sf::RenderWindow& window) {
    SpriteSheet& spriteSheet = getCurrentSpriteSheet();

    spriteSheet.m_sprite.setPosition(m_position);
    spriteSheet.draw(window);
}

void Player::movementUpdate(float deltaTime) {
    bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);

    m_movement.x = rightPressed - leftPressed;
    m_movement.y = downPressed - upPressed;

    m_position += m_movement * m_movementSpeed * deltaTime;
}

void Player::animationUpdate(float deltaTime) {
    static bool wasMoving = false;
    static bool hadFootDown = false;
    static bool facingRight = true;
    static bool facingForward = true;

    facingRight   = (facingRight   || m_movement.x > 0.1f) && m_movement.x >= -0.1f;
    facingForward = (facingForward || m_movement.y > 0.1f) && m_movement.y >= -0.1f;
    m_moving = std::abs(m_movement.x) > 0.1f || std::abs(m_movement.y) > 0.1f;

    // reset the looping sprite sheet that is not currently in use, so it doesn't start half way through
    if (m_moving) m_idleSpriteSheet.setIndex(0);
    else          m_walkSpriteSheet.setIndex(0);

    m_footDown = m_moving && (m_walkSpriteSheet.getIndex() % 2 == 1);

    if (m_footDown && !hadFootDown) SoundManager::get().playSound(*r_stepSound);

    // set the sprite sheets to use the correct animation for the facing direction
    float rowIndex = (!facingForward << 1) + !facingRight;
    m_idleSpriteSheet.m_animationRegion.top = rowIndex / 4.f;
    m_walkSpriteSheet.m_animationRegion.top = rowIndex / 4.f;
    m_attackSpriteSheet.m_animationRegion.top = rowIndex / 4.f;

    // animate sprite sheets
    m_idleSpriteSheet.incrementIndex(deltaTime);
    m_walkSpriteSheet.incrementIndex(deltaTime);
    m_attackSpriteSheet.incrementIndex(deltaTime);

    m_attacking &= !m_attackSpriteSheet.hasFinished();

    wasMoving = m_moving;
    hadFootDown = m_footDown;
}

void Player::tileSetCollisionUpdate(TileSet& tileSet) {
    sf::Vector2i offset, playerCell = tileSet.getCellAtPosition(m_position);
    for (offset.y = -1; offset.y <= 1; offset.y++)
    for (offset.x = -1; offset.x <= 1; offset.x++)
    if (!(offset.x && offset.y)) {
        sf::Vector2i cell {
            playerCell.x + offset.x,
            playerCell.y + offset.y
        };

        sf::FloatRect playerBounds = getBounds();
        sf::FloatRect tileBounds = tileSet.getCellBounds(cell);

        if (tileSet.isWallType(tileSet.getCellType(cell)) &&
            playerBounds.intersects(tileBounds))
        {
            sf::Vector2f tileCentre = tileBounds.getPosition() + tileBounds.getSize() * 0.5f;
            sf::Vector2f deltaPosition = m_position - tileCentre;

            if (std::abs(deltaPosition.x) > std::abs(deltaPosition.y)) {
                if (m_position.x < tileCentre.x) m_position.x = tileBounds.left - playerBounds.width * 0.5f;
                else m_position.x = tileBounds.left + tileBounds.width + playerBounds.width * 0.5f;
            } else {
                if (m_position.y < tileCentre.y) m_position.y = tileBounds.top - playerBounds.height * 0.5f;
                else m_position.y = tileBounds.top + tileBounds.height + playerBounds.height * 0.5f;
            }
        }
    }
}

void Player::attack() {
    if (m_attacking) return;

    m_attacking = true;
    m_attackSpriteSheet.setIndex(0);
    SoundManager::get().playSound(*r_attackSound);
}
