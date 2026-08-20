#include "SaveManager.h"
#include <fstream>
#include <iostream>

bool SaveManager::save(const SaveData& data, const std::string& path)
{
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cout << "[Save] Khong ghi duoc file: " << path << "\n";
        return false;
    }

    file << data.bestScore << "\n";
    file << data.highestUnlockedChapter << "\n";
    file << static_cast<int>(data.theme) << "\n";
    file << data.musicVolume << "\n";
    file << data.sfxVolume << "\n";

    file.close();
    return true;
}

bool SaveManager::load(SaveData& data, const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        // Chưa có file save → dùng giá trị mặc định
        return false;
    }

    int themeInt = 0;
    file >> data.bestScore;
    file >> data.highestUnlockedChapter;
    file >> themeInt;
    file >> data.musicVolume;
    file >> data.sfxVolume;

    data.theme = static_cast<ThemeType>(themeInt);

    // Bảo vệ giá trị lỗi
    if (data.highestUnlockedChapter < 0) data.highestUnlockedChapter = 0;
    if (data.highestUnlockedChapter > 4) data.highestUnlockedChapter = 4;
    if (data.bestScore < 0) data.bestScore = 0;

    file.close();
    return true;
}