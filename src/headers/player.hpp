#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <spriteSheet.hpp>
#include <soundManager.hpp>
#include <tileSet.hpp>

class Player {
    SpriteSheet m_attackSpriteSheet,
                m_idleSpriteSheet,
                m_walkSpriteSheet;
    
    bool m_moving = false;
    bool m_footDown = false;
    bool m_attacking = false;
    sf::Vector2f m_movement;

    float m_movementSpeed = 200.f;

    static constexpr float s_movementThreshold = 0.25f;

    class Resources {
        sf::SoundBuffer *r_attackSound,
                        *r_stepSound;
        
        sf::Texture m_attackSpriteSheetTexture,
                    m_idleSpriteSheetTexture,
                    m_walkSpriteSheetTexture;
        
        static Resources s_singleton;

        static constexpr char s_attackSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Attack.png";
        static constexpr char s_idleSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Idle.png";
        static constexpr char s_walkSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Walk.png";

        static constexpr char s_attackSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/07_human_atk_sword_1.wav";
        static constexpr char s_stepSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/16_human_walk_stone_1.wav";

        Resources() {
            r_attackSound = &SoundManager::get().loadSound(s_attackSoundPath);
            r_stepSound = &SoundManager::get().loadSound(s_stepSoundPath);

            loadTexture(m_attackSpriteSheetTexture, s_attackSpriteSheetPath);
            loadTexture(m_idleSpriteSheetTexture, s_idleSpriteSheetPath);
            loadTexture(m_walkSpriteSheetTexture, s_walkSpriteSheetPath);
        }

        Resources(const Resources& other) = delete;
        Resources(Resources&& other) = delete;
        Resources& operator=(const Resources& other) = delete;
        Resources& operator=(Resources&& other) = delete;

        void loadTexture(sf::Texture& texture, const std::string& filename) {
            if (!texture.loadFromFile(filename))
                throw std::runtime_error("Failed to load texture from file: " + filename);
        }
    
    public:
        static Resources& get() { return s_singleton; }

        sf::SoundBuffer& attackSound() { return *r_attackSound; };
        sf::SoundBuffer& stepSound() { return *r_stepSound; }
        sf::Texture& attackSpriteSheetTexture() { return m_attackSpriteSheetTexture; }
        sf::Texture& idleSpriteSheetTexture() { return m_idleSpriteSheetTexture; }
        sf::Texture& walkSpriteSheetTexture() { return m_walkSpriteSheetTexture; }
    };

public:
    sf::Vector2f m_position;

    Player();

    sf::FloatRect getBounds();
    SpriteSheet& getCurrentSpriteSheet();
    sf::Vector2f getFacingDirection();

    const sf::Vector2f& getMovement() { return m_movement; }

    void draw(sf::RenderWindow& window);

    void movementUpdate(float deltaTime);
    void tileSetCollisionUpdate(TileSet& tileSet);
    void animationUpdate(float deltaTime);

    void attack();
};
