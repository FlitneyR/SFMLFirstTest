#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <soundManager.hpp>
#include <spriteSheet.hpp>
#include <tileSet.hpp>

class Orc {
    class Resources {
        static Resources s_singleton;

        sf::Texture m_idleSpriteSheetTexture,
                    m_walkSpriteSheetTexture,
                    m_damageSpriteSheetTexture,
                    m_attackSpriteSheetTexture;

        sf::SoundBuffer *r_attackSound,
                        *r_stepSound,
                        *r_damageSound;

        bool m_loaded = false;

        static constexpr char s_idleSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Orc/Idle.png";
        static constexpr char s_walkSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Orc/Walk.png";
        static constexpr char s_damageSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Orc/Dmg.png";
        static constexpr char s_attackSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Orc/Attack.png";

        static constexpr char s_attackSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/17_orc_atk_sword_1.wav";
        static constexpr char s_walkSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/25_orc_walk_stone_1.wav";
        static constexpr char s_damageSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/21_orc_damage_3.wav";

        void loadResources() {
            if (m_loaded) return;

            r_attackSound = &SoundManager::get().loadSound(s_attackSoundPath);
            r_stepSound = &SoundManager::get().loadSound(s_walkSoundPath);
            r_damageSound = &SoundManager::get().loadSound(s_damageSoundPath);

            loadTexture(m_idleSpriteSheetTexture, s_idleSpriteSheetPath);
            loadTexture(m_attackSpriteSheetTexture, s_attackSpriteSheetPath);
            loadTexture(m_walkSpriteSheetTexture, s_walkSpriteSheetPath);
            loadTexture(m_damageSpriteSheetTexture, s_damageSpriteSheetPath);

            m_loaded = true;
        }

        void loadTexture(sf::Texture& texture, const std::string& filename) {
            if (!texture.loadFromFile(filename))
                throw std::runtime_error("Failed to load texture from file: " + filename);
        }

    public:
        static Resources& get() {
            s_singleton.loadResources();
            return s_singleton;
        }

        sf::Texture& idleSpriteSheetTexture() { return m_idleSpriteSheetTexture; }
        sf::Texture& walkSpriteSheetTexture() { return m_walkSpriteSheetTexture; }
        sf::Texture& damageSpriteSheetTexture() { return m_damageSpriteSheetTexture; }
        sf::Texture& attackSpriteSheetTexture() { return m_attackSpriteSheetTexture; }
        sf::SoundBuffer& attackSound() { return *r_attackSound; }
        sf::SoundBuffer& stepSound() { return *r_stepSound; }
        sf::SoundBuffer& damageSound() { return *r_damageSound; }
    };

    float m_movementSpeed = 200.f;
    static constexpr float s_movementThreshold = 0.25f;

    SpriteSheet m_idleSpriteSheet;
    SpriteSheet m_walkSpriteSheet;
    SpriteSheet m_damageSpriteSheet;
    SpriteSheet m_attackSpriteSheet;

    float m_health = 10.f;

    bool m_moving = false;
    bool m_attacking = false;
    bool m_takingDamage = false;

    float m_attackCooldown = 0.f;
    static constexpr float s_attackCooldown = 1.f;

public:
    sf::Vector2f m_position {};
    sf::Vector2f m_movement {};

    Orc();

    sf::Vector2f getFacingDirection();
    sf::FloatRect getBounds();
    SpriteSheet& getCurrentSpriteSheet();

    bool runTowards(sf::Vector2f target);

    void movementUpdate(float deltaTime);
    void tileSetCollisionUpdate(TileSet& tileSet);
    void updateAnimation(float deltaTime);

    void draw(sf::RenderTarget& renderTarget);

    void takeDamage(float damage);
    void attack();
    
    bool isAlive() const { return m_health > 0.f; }
    bool canTakeDamage() const { return !m_takingDamage; }
    bool isAttacking() const { return m_attacking; }

    bool canAttack() const {
        return !(isAttacking() || m_attackCooldown > 0.f);
    }
};