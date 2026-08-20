#pragma once
#include <string>
#include "Settings.h"   // để dùng ThemeType

struct SaveData {
    int bestScore = 0;
    int highestUnlockedChapter = 0;   // 0 = chapter 1
    ThemeType theme = THEME_GREEN;
    float musicVolume = 50.f;
    float sfxVolume = 50.f;
};

class SaveManager {
public:
    static bool save(const SaveData& data, const std::string& path = "save.dat");
    static bool load(SaveData& data, const std::string& path = "save.dat");
};