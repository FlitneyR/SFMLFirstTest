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

void Player::draw(sf::RenderWindow& window) {
    SpriteSheet& spriteSheet =
        m_attacking ? m_attackSpriteSheet :
        m_moving    ? m_walkSpriteSheet   :
                      m_idleSpriteSheet   ;

    spriteSheet.m_sprite.setPosition(m_position);
    spriteSheet.draw(window);
}

void Player::update(float deltaTime) {
    static bool wasMoving = false;
    static bool hadFootDown = false;
    static bool facingRight = true;
    static bool facingForward = true;

    bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);

    m_movement.x = rightPressed - leftPressed;
    m_movement.y = downPressed - upPressed;

    if (m_movement.x > 0) facingRight = true;
    if (m_movement.x < 0) facingRight = false;
    if (m_movement.y > 0) facingForward = true;
    if (m_movement.y < 0) facingForward = false;

    m_moving = std::abs(m_movement.x) > 0.1f || std::abs(m_movement.y) > 0.1f;

    if (m_moving) m_idleSpriteSheet.setIndex(0);
    else          m_walkSpriteSheet.setIndex(0);

    m_footDown = m_moving && (m_walkSpriteSheet.getIndex() % 2 == 1);

    bool justStepped = m_footDown && !hadFootDown;
    if (justStepped)
        SoundManager::get().playSound(*r_stepSound);

    float rowIndex = (!facingForward << 1) + !facingRight;
    m_idleSpriteSheet.m_animationRegion.top = rowIndex / 4.f;
    m_walkSpriteSheet.m_animationRegion.top = rowIndex / 4.f;
    m_attackSpriteSheet.m_animationRegion.top = rowIndex / 4.f;

    m_position += m_movement * m_movementSpeed * deltaTime;

    m_idleSpriteSheet.incrementIndex(deltaTime);
    m_walkSpriteSheet.incrementIndex(deltaTime);
    m_attackSpriteSheet.incrementIndex(deltaTime);

    m_attacking &= !m_attackSpriteSheet.hasFinished();

    wasMoving = m_moving;
    hadFootDown = m_footDown;
}

void Player::attack() {
    if (m_attacking) return;
    
    m_attacking = true;
    m_attackSpriteSheet.setIndex(0);
    SoundManager::get().playSound(*r_attackSound);
}
