#include "AudioManager.h"
#include <iostream>
#include <algorithm>

AudioManager::AudioManager()
    : musicVolume(50.0f)
    , sfxVolume(70.0f)   
    , sfxPool{
        sf::Sound(dummyBuffer), sf::Sound(dummyBuffer),
        sf::Sound(dummyBuffer), sf::Sound(dummyBuffer),
        sf::Sound(dummyBuffer), sf::Sound(dummyBuffer),
        sf::Sound(dummyBuffer), sf::Sound(dummyBuffer)
    }
    , nextSfxIndex(0)
    , currentMusic(MusicType::NONE)
{
    musicLoading.setVolume(musicVolume);
    musicMain.setVolume(musicVolume);

    for (auto& s : sfxPool)
        s.setVolume(sfxVolume);

    // NAP 6 SFX 
    loadSound("click", "assets/sounds/sfx/click.wav");
    loadSound("merge_small", "assets/sounds/sfx/merge_small.wav");
    loadSound("merge_large", "assets/sounds/sfx/merge_large.wav");
    loadSound("win", "assets/sounds/sfx/win.wav");
    loadSound("lose", "assets/sounds/sfx/lose.wav");
    loadSound("milestone", "assets/sounds/sfx/milestone.wav");
}


// MUSIC
void AudioManager::playLoadingMusic(bool loop)
{
    if (currentMusic == MusicType::LOADING &&
        musicLoading.getStatus() == sf::Music::Status::Playing)
        return;

    stopMusic();

    if (musicLoading.openFromFile("assets/sounds/music/loading.ogg")) {
        musicLoading.setLooping(loop);
        musicLoading.setVolume(musicVolume);
        musicLoading.play();
        currentMusic = MusicType::LOADING;
        std::cout << "[Audio] Playing loading music\n";
    }
    else {
        std::cout << "[Loi Audio] Khong mo duoc loading.ogg\n";
    }
}

void AudioManager::playMainMusic(bool loop)
{
    if (currentMusic == MusicType::MAIN &&
        musicMain.getStatus() == sf::Music::Status::Playing)
        return;

    stopMusic();

    if (musicMain.openFromFile("assets/sounds/music/main.ogg")) {
        musicMain.setLooping(loop);
        musicMain.setVolume(musicVolume);
        musicMain.play();
        currentMusic = MusicType::MAIN;
        std::cout << "[Audio] Playing main music\n";
    }
    else {
        std::cout << "[Loi Audio] Khong mo duoc main.ogg\n";
    }
}

void AudioManager::stopMusic()
{
    musicLoading.stop();
    musicMain.stop();
    currentMusic = MusicType::NONE;
}

void AudioManager::pauseMusic()
{
    if (currentMusic == MusicType::LOADING)
        musicLoading.pause();
    else if (currentMusic == MusicType::MAIN)
        musicMain.pause();
}

void AudioManager::resumeMusic()
{
    if (currentMusic == MusicType::LOADING)
        musicLoading.play();
    else if (currentMusic == MusicType::MAIN)
        musicMain.play();
}

// SFX
void AudioManager::loadSound(const std::string& name, const std::string& filepath)
{
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filepath)) {
        soundBuffers[name] = std::move(buffer);
        std::cout << "[Audio] Loaded SFX: " << name << " (" << filepath << ")\n";
    }
    else {
        std::cout << "[Loi Audio] Khong tai duoc SFX: " << filepath
            << "  <- kiem tra path / Working Directory\n";
    }
}

void AudioManager::playSound(const std::string& name)
{
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        std::cout << "[Loi Audio] Khong tim thay SFX: " << name
            << " (load that bai luc khoi tao?)\n";
        return;
    }

    // Lay 1 slot trong pool (xoay vong)
    sf::Sound& sound = sfxPool[nextSfxIndex];
    nextSfxIndex = (nextSfxIndex + 1) % SFX_POOL_SIZE;

    // Stop truoc khi doi buffer
    sound.stop();
    sound.setBuffer(it->second);

    // SFML volume CHI nhan 0..100
    float vol = std::clamp(sfxVolume, 0.0f, 100.0f);
    sound.setVolume(vol);
    sound.play();
}

// VOLUME
void AudioManager::setMusicVolume(float volume)
{
    musicVolume = std::clamp(volume, 0.0f, 100.0f);
    musicLoading.setVolume(musicVolume);
    musicMain.setVolume(musicVolume);
}

void AudioManager::setSfxVolume(float volume)
{
    sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    for (auto& s : sfxPool)
        s.setVolume(sfxVolume);
}
