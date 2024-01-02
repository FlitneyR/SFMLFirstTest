#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <spriteSheet.hpp>
#include <soundManager.hpp>
#include <tileSet.hpp>
#include <optional>

class Player {
    class Resources {
        sf::SoundBuffer *r_attackSound,
                        *r_stepSound;
        
        sf::Texture m_attackSpriteSheetTexture,
                    m_idleSpriteSheetTexture,
                    m_walkSpriteSheetTexture;
        
        bool m_loaded = false;
        
        static Resources s_singleton;

        static constexpr char s_attackSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Attack.png";
        static constexpr char s_idleSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Idle.png";
        static constexpr char s_walkSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Walk.png";

        static constexpr char s_attackSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/07_human_atk_sword_1.wav";
        static constexpr char s_stepSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/16_human_walk_stone_1.wav";

        Resources() = default;
        
        Resources(const Resources& other) = delete;
        Resources(Resources&& other) = delete;
        Resources& operator=(const Resources& other) = delete;
        Resources& operator=(Resources&& other) = delete;

        void loadResources() {
            if (m_loaded) return;

            r_attackSound = &SoundManager::get().loadSound(s_attackSoundPath);
            r_stepSound = &SoundManager::get().loadSound(s_stepSoundPath);

            loadTexture(m_attackSpriteSheetTexture, s_attackSpriteSheetPath);
            loadTexture(m_idleSpriteSheetTexture, s_idleSpriteSheetPath);
            loadTexture(m_walkSpriteSheetTexture, s_walkSpriteSheetPath);

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

        sf::SoundBuffer& attackSound() { return *r_attackSound; };
        sf::SoundBuffer& stepSound() { return *r_stepSound; }
        sf::Texture& attackSpriteSheetTexture() { return m_attackSpriteSheetTexture; }
        sf::Texture& idleSpriteSheetTexture() { return m_idleSpriteSheetTexture; }
        sf::Texture& walkSpriteSheetTexture() { return m_walkSpriteSheetTexture; }
    };

    SpriteSheet m_attackSpriteSheet,
                m_idleSpriteSheet,
                m_walkSpriteSheet;
    
    bool m_moving = false;
    bool m_footDown = false;
    bool m_attacking = false;
    sf::Vector2f m_movement;

    float m_movementSpeed = 200.f;

    static constexpr float s_movementThreshold = 0.25f;

public:
    sf::Vector2f m_position;

    Player();

    sf::FloatRect getBounds() const;
    SpriteSheet& getCurrentSpriteSheet();
    sf::Vector2f getFacingDirection() const;

    const sf::Vector2f& getMovement() const { return m_movement; }

    void draw(sf::RenderTarget& target);

    void movementUpdate(float deltaTime);
    void tileSetCollisionUpdate(TileSet& tileSet);
    void animationUpdate(float deltaTime);

    void attack();

    std::optional<sf::FloatRect> getSwordBounds() const;
    bool isSwordCollidingWith(const sf::FloatRect& bounds) const;
};
