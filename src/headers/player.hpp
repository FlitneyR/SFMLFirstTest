#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <spriteSheet.hpp>
#include <soundManager.hpp>

class Player {
    sf::SoundBuffer *r_attackSound,
                    *r_stepSound;

    SpriteSheet m_attackSpriteSheet,
                m_idleSpriteSheet,
                m_walkSpriteSheet;
    
    bool m_moving = false;
    bool m_footDown = false;
    bool m_attacking = false;

    sf::Vector2f m_movement;

    float m_movementSpeed = 200.f;

    static constexpr char s_attackSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Attack.png";
    static constexpr char s_idleSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Idle.png";
    static constexpr char s_walkSpriteSheetPath[] = "../assets/sprites/Minifantasy_Dungeon_v2.2_Free_Version/Minifantasy_Dungeon_Assets/Characters/Human/Walk.png";

    static constexpr char s_attackSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/07_human_atk_sword_1.wav";
    static constexpr char s_walkSoundPath[] = "../assets/audio/Minifantasy_Dungeon_SFX/16_human_walk_stone_1.wav";

public:
    sf::Vector2f m_position {};

    Player();

    void draw(sf::RenderWindow& window);
    void update(float deltaTime);
    void attack();
};
