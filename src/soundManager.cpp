#include <soundManager.hpp>
#include <iostream>

SoundManager SoundManager::s_singleton {};

void SoundManager::log() const {
    std::cout << m_playingMusic.size() << " music tracks and " << m_playingSounds.size() << " sounds playing" << std::endl;
}

SoundManager& SoundManager::get() {
    return s_singleton;
}

sf::SoundBuffer& SoundManager::loadSound(const std::string& fileName) {
    auto it = m_soundBuffers.find(fileName);

    if (it == m_soundBuffers.end()) {
        sf::SoundBuffer& soundBuffer = m_soundBuffers[fileName];

        if (!soundBuffer.loadFromFile(fileName))
            throw std::runtime_error("Couldn't load sound from: " + fileName);
        
        return soundBuffer;
    } else {
        return it->second;
    }
}

void SoundManager::playSound(sf::SoundBuffer& soundBuffer) {
    // auto sound = std::make_unique<sf::Sound>(soundBuffer);
    // sf::Sound sound(soundBuffer);
    // sound->play();
    // m_playingSounds.push_back(std::move(sound));
    if (m_playingSounds.size() < 100) {
        m_playingSounds.emplace_back(soundBuffer);
        m_playingSounds.back().play();
    }
}

sf::Music& SoundManager::playMusic(const std::string& fileName) {
    // auto music = std::make_unique<sf::Music>();
    m_playingMusic.emplace_back();
    sf::Music& music = m_playingMusic.back();

    if (!music.openFromFile(fileName))
        throw std::runtime_error("Failed to load music from: " + fileName);

    music.play();
    music.setLoop(true);
    return music;
}

void SoundManager::cleanUpFinishedSounds() {
    for (auto it = m_playingSounds.begin(); it != m_playingSounds.end();) {
        if (it->getStatus() == sf::SoundSource::Status::Stopped) {
            m_playingSounds.erase(it++);
        } else {
            it++;
        }
    }
}
