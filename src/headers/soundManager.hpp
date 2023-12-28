#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <list>

class SoundManager {
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::list<sf::Sound> m_playingSounds;
    std::list<sf::Music> m_playingMusic;

    SoundManager() = default;

    static SoundManager s_singleton;

public:
    // Explicitly delete move and copy constructors and assignment operators.
    // You should only ever use a reference to the static singleton that you
    // get from SoundManager& SoundManager::get().
    // But keep these deleted functions public to give a more helpful
    // error message.
    SoundManager(const SoundManager& other) = delete;
    SoundManager(const SoundManager&& other) = delete;
    SoundManager& operator=(const SoundManager& other) = delete;
    SoundManager& operator=(const SoundManager&& other) = delete;

    static SoundManager& get();

    sf::SoundBuffer& loadSound(const std::string& fileName);
    void playSound(sf::SoundBuffer& soundBuffer);
    void playMusic(const std::string& fileName);
    void cleanUpFinishedSounds();
};
