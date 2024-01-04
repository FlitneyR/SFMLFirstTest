#include <orc.hpp>
#include <iostream>

Orc::Resources Orc::Resources::s_singleton {};

Orc::Orc() :
    m_idleSpriteSheet(
        Resources::get().idleSpriteSheetTexture(),
        1, 16, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_walkSpriteSheet(
        Resources::get().walkSpriteSheetTexture(),
        1, 4, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_damageSpriteSheet(
        Resources::get().damageSpriteSheetTexture(),
        1, 4, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    ),
    m_attackSpriteSheet(
        Resources::get().attackSpriteSheetTexture(),
        1, 4, { { 0.f, 0.f }, { 1.f, 1.f / 4.f } }
    )
{
    m_idleSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_idleSpriteSheet.m_animationSpeed = 5.f;
    
    m_walkSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_walkSpriteSheet.m_animationSpeed = 5.f;
    
    m_damageSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_damageSpriteSheet.m_animationSpeed = 10.f;
    m_damageSpriteSheet.m_loop = false;
    
    m_attackSpriteSheet.m_sprite.setScale({ 5.f, 5.f });
    m_attackSpriteSheet.m_animationSpeed = 10.f;
    m_attackSpriteSheet.m_loop = false;
}

void Orc::preventIntersection(std::vector<Orc>& orcs, float deltaTime) {
    for (int i = 0; i + 1 < orcs.size(); i++)
    for (int j = i + 1; j < orcs.size(); j++) {
        Orc& orc1 = orcs[i];
        Orc& orc2 = orcs[j];

        const sf::FloatRect& orc1Bounds = orc1.getBounds();
        const sf::FloatRect& orc2Bounds = orc2.getBounds();

        if (orc1Bounds.intersects(orc2Bounds)) {
            sf::Vector2f deltaPosition = orc2.m_position - orc1.m_position;

            deltaPosition.x = deltaPosition.x > 0.f ?
                orc1Bounds.left + orc1Bounds.width - orc2Bounds.left :
                orc2Bounds.left + orc2Bounds.width - orc1Bounds.left ;
            
            deltaPosition.y = deltaPosition.y > 0.f ?
                orc1Bounds.top + orc1Bounds.height - orc2Bounds.top :
                orc2Bounds.top + orc2Bounds.height - orc1Bounds.top ;
            
            if (std::abs(deltaPosition.x) > std::abs(deltaPosition.y))
                deltaPosition.x = 0.f;
            else
                deltaPosition.y = 0.f;

            orc1.m_position += deltaPosition * deltaTime * 15.f;
            orc2.m_position -= deltaPosition * deltaTime * 15.f;
        }
    }
}

sf::Vector2f Orc::getFacingDirection() {
    static bool facingRight = true;
    static bool facingForward = true;

    facingRight   = (facingRight   || m_movement.x > s_movementThreshold) && (m_movement.x >= -s_movementThreshold);
    facingForward = (facingForward || m_movement.y > s_movementThreshold) && (m_movement.y >= -s_movementThreshold);

    return {
        facingRight ? 1.f : -1.f,
        facingForward ? 1.f : -1.f
    };
}

sf::FloatRect Orc::getBounds() {
    return {
        { m_position.x - 15.f, m_position.y - 15.f },
        { 30.f, 30.f }
    };
}

SpriteSheet& Orc::getCurrentSpriteSheet() {
    return
        m_takingDamage   ? m_damageSpriteSheet :
        m_attacking      ? m_attackSpriteSheet :
        m_moving         ? m_walkSpriteSheet   :
                           m_idleSpriteSheet   ;
}

void Orc::updateAnimation(float deltaTime) {
    static bool hadFootDown = false;
    
    m_moving = std::sqrt(m_movement.x * m_movement.x + m_movement.y * m_movement.y) >= s_movementThreshold;

    bool footDown = m_moving && (m_walkSpriteSheet.getIndex() % 2 == 1);

    if (footDown && !hadFootDown)
        SoundManager::get().playSound(Resources::get().stepSound());

    sf::Vector2f facingDirection = getFacingDirection();
    float animationIndex = ((facingDirection.y < 0.f) << 1u) | (facingDirection.x < 0.f);
    m_idleSpriteSheet.m_animationRegion.top = animationIndex / 4.f;
    m_walkSpriteSheet.m_animationRegion.top = animationIndex / 4.f;
    m_damageSpriteSheet.m_animationRegion.top = animationIndex / 4.f;
    m_attackSpriteSheet.m_animationRegion.top = animationIndex / 4.f;

    m_idleSpriteSheet.incrementIndex(deltaTime);
    m_walkSpriteSheet.incrementIndex(deltaTime);
    m_damageSpriteSheet.incrementIndex(deltaTime);
    m_attackSpriteSheet.incrementIndex(deltaTime);

    m_takingDamage &= !m_damageSpriteSheet.hasFinished();
    m_attacking &= !m_attackSpriteSheet.hasFinished();

    if (!m_takingDamage) m_damageSpriteSheet.setIndex(0);
    if (!m_attacking)    m_attackSpriteSheet.setIndex(0);
    if (!m_moving)       m_walkSpriteSheet.setIndex(0);

    if (!(m_takingDamage || m_moving)) m_walkSpriteSheet.setIndex(0);

    m_attackCooldown = std::max(0.f, m_attackCooldown - deltaTime);

    hadFootDown = footDown;
}

bool Orc::runTowards(sf::Vector2f target) {
    sf::Vector2f diff = target - m_position;

    float length = std::sqrt(
        diff.x * diff.x +
        diff.y * diff.y);

    if (length > 100.f) {
        m_movement = diff / length;
        return false;
    } else {
        m_movement *= 0.9f;
        return true;
    }
}

void Orc::movementUpdate(float deltaTime) {
    m_position += m_movement * m_movementSpeed * deltaTime;
}

void Orc::tileSetCollisionUpdate(TileSet& tileSet) {
    sf::FloatRect bounds = getBounds();

    sf::Vector2i offset, orcCell = tileSet.getCellAtPosition(m_position);
    for (offset.y = -1; offset.y <= 1; offset.y++)
    for (offset.x = -1; offset.x <= 1; offset.x++)
    if (!(offset.x && offset.y)) {
        sf::Vector2i cell = orcCell + offset;

        sf::FloatRect tileBounds = tileSet.getCellBounds(cell);

        if (tileSet.isWallType(tileSet.getCellType(cell)) && bounds.intersects(tileBounds)) {
            sf::Vector2f tileCentre = tileBounds.getPosition() + tileBounds.getSize() * 0.5f;
            sf::Vector2f deltaPosition = m_position - tileCentre;

            if (std::abs(deltaPosition.x) > std::abs(deltaPosition.y)) {
                if (m_position.x < tileCentre.x) m_position.x = tileBounds.left - bounds.width * 0.5f;
                else m_position.x = tileBounds.left + tileBounds.width + bounds.width * 0.5f;
            } else {
                if (m_position.y < tileCentre.y) m_position.y = tileBounds.top - bounds.height * 0.5f;
                else m_position.y = tileBounds.top + tileBounds.height + bounds.height * 0.5f;
            }
        }
    }
}

void Orc::draw(sf::RenderTarget& renderTarget) {
    SpriteSheet& spriteSheet = getCurrentSpriteSheet();
    spriteSheet.m_sprite.setPosition(m_position);
    spriteSheet.draw(renderTarget);
}

void Orc::takeDamage(float damage) {
    m_health -= damage;

    m_takingDamage = true;
    SoundManager::get().playSound(Resources::get().damageSound());
}

void Orc::attack() {
    if (!canAttack()) return;

    m_attacking = true;
    SoundManager::get().playSound(Resources::get().attackSound());

    m_attackCooldown = s_attackCooldown;
}


