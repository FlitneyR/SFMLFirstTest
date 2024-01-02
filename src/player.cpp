#include <player.hpp>

Player::Resources Player::Resources::s_singleton {};

Player::Player() :
    m_attackSpriteSheet(
        Resources::get().attackSpriteSheetTexture(),
        1, 4, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_walkSpriteSheet(
        Resources::get().walkSpriteSheetTexture(),
        1, 4, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_idleSpriteSheet(
        Resources::get().idleSpriteSheetTexture(),
        1, 16, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    )
{
    m_attackSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_attackSpriteSheet.m_animationSpeed = 10.f;
    m_attackSpriteSheet.m_loop = false;

    m_idleSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_idleSpriteSheet.m_animationSpeed = 5.f;

    m_walkSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_walkSpriteSheet.m_animationSpeed = 5.f;
}

sf::FloatRect Player::getBounds() const {
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

sf::Vector2f Player::getFacingDirection() const {
    static bool facingRight = true;
    static bool facingForward = true;

    facingRight   = (facingRight   || m_movement.x > s_movementThreshold) && m_movement.x >= -s_movementThreshold;
    facingForward = (facingForward || m_movement.y > s_movementThreshold) && m_movement.y >= -s_movementThreshold;

    return {
        facingRight ? 1.f : -1.f,
        facingForward ? 1.f : -1.f
    };
}

void Player::draw(sf::RenderTarget& target) {
    SpriteSheet& spriteSheet = getCurrentSpriteSheet();
    spriteSheet.m_sprite.setPosition(m_position);
    spriteSheet.draw(target);
}

void Player::movementUpdate(float deltaTime) {
    bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);

    m_movement.x += static_cast<float>(rightPressed - leftPressed) * deltaTime * 5.f;
    m_movement.y += static_cast<float>(downPressed - upPressed) * deltaTime * 5.f;
    
    float movementLength = std::sqrt(m_movement.x * m_movement.x + m_movement.y * m_movement.y);
    if (movementLength > 1.f) m_movement /= movementLength;

    m_position += m_movement * m_movementSpeed * deltaTime;

    float friction = 1.f / (10.f * deltaTime + 1.f);

    if (!(leftPressed || rightPressed)) m_movement.x *= friction;
    if (!(upPressed || downPressed)) m_movement.y *= friction;
}

void Player::animationUpdate(float deltaTime) {
    static bool hadFootDown = false;

    m_moving = std::sqrt(m_movement.x * m_movement.x + m_movement.y * m_movement.y) >= s_movementThreshold;

    m_footDown = m_moving && (m_walkSpriteSheet.getIndex() % 2 == 1);

    if (m_footDown && !hadFootDown)
        SoundManager::get().playSound(Resources::get().stepSound());

    // set the sprite sheets to use the correct animation for the facing direction
    sf::Vector2f facing = getFacingDirection();
    float rowIndex = ((facing.y < 0) << 1) | (facing.x < 0);
    m_idleSpriteSheet.m_animationRegion.top = rowIndex / 4.f;
    m_walkSpriteSheet.m_animationRegion.top = rowIndex / 4.f;
    m_attackSpriteSheet.m_animationRegion.top = rowIndex / 4.f;

    // animate sprite sheets
    m_idleSpriteSheet.incrementIndex(deltaTime);
    m_walkSpriteSheet.incrementIndex(deltaTime);
    m_attackSpriteSheet.incrementIndex(deltaTime);

    m_attacking &= !m_attackSpriteSheet.hasFinished();

    // reset the looping sprite sheet that is not currently in use, so it doesn't start half way through
    if (m_moving) m_idleSpriteSheet.setIndex(0);
    else          m_walkSpriteSheet.setIndex(0);

    hadFootDown = m_footDown;
}

void Player::tileSetCollisionUpdate(TileSet& tileSet) {
    sf::FloatRect playerBounds = getBounds();

    sf::Vector2i offset, playerCell = tileSet.getCellAtPosition(m_position);
    for (offset.y = -1; offset.y <= 1; offset.y++)
    for (offset.x = -1; offset.x <= 1; offset.x++)
    if (!(offset.x && offset.y)) {
        sf::Vector2i cell = playerCell + offset;

        sf::FloatRect tileBounds = tileSet.getCellBounds(cell);

        if (tileSet.isWallType(tileSet.getCellType(cell)) && playerBounds.intersects(tileBounds)) {
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

std::optional<sf::FloatRect> Player::getSwordBounds() const {
    if (!m_attacking) return {};
    if (m_attackSpriteSheet.getIndex() <= 1) return {};
    if (m_attackSpriteSheet.getIndex() >= 3) return {};

    sf::Vector2f swordBoundsSize { 50.f, 40.f };

    sf::Vector2f swordBoundsOffset = getFacingDirection();
    swordBoundsOffset.x *= 18.f;
    swordBoundsOffset.y *= 7.f;
    swordBoundsOffset.y += 3.f;

    return sf::FloatRect {
        m_position + swordBoundsOffset
                   - swordBoundsSize * 0.5f,
        swordBoundsSize
    };
}

bool Player::isSwordCollidingWith(const sf::FloatRect& bounds) const {
    if (auto swordBounds = getSwordBounds())
        return swordBounds->intersects(bounds);
    else return false;
}

void Player::attack() {
    if (m_attacking) return;

    m_attacking = true;
    m_attackSpriteSheet.setIndex(0);
    SoundManager::get().playSound(Resources::get().attackSound());
}
