#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <array>

class AudioManager {
private:
    // MUSIC 
    sf::Music musicLoading;   // Nhac Loading Screen
    sf::Music musicMain;      // Nhac Main Menu + Gameplay + Challenge

    // SFX 
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;

    // SFML 3.1: Sound bat buoc co buffer khi khoi tao
    sf::SoundBuffer dummyBuffer;

    // Pool nhieu Sound de SFX chong len nhau (click lien tuc khong bi cat)
    static constexpr int SFX_POOL_SIZE = 8;
    std::array<sf::Sound, SFX_POOL_SIZE> sfxPool;
    int nextSfxIndex = 0;

    float musicVolume;
    float sfxVolume;

    // Trang thai dang phat nhac nao
    enum class MusicType { NONE, LOADING, MAIN };
    MusicType currentMusic;

public:
    AudioManager();

    //  MUSIC 
    void playLoadingMusic(bool loop = true);
    void playMainMusic(bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    //  SFX
    void loadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name);

    // VOLUME 
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);

    float getMusicVolume() const { return musicVolume; }
    float getSfxVolume()   const { return sfxVolume; }
};
