#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "VolumeSlider.h"
#include "Button.h"

// Định nghĩa các loại Theme
enum ThemeType {
    THEME_GREEN,
    THEME_BLUE,
    THEME_PINK
};

// Helper chuyển ThemeType → tên thư mục
inline std::string getThemeFolder(ThemeType theme) {
    switch (theme) {
    case THEME_BLUE: return "blue";
    case THEME_PINK: return "pink";
    case THEME_GREEN:
    default:         return "green";
    }
}

class SettingsPopup {
private:
    sf::RectangleShape dimOverlay;
    sf::Texture bgTexture;
    sf::Sprite  bgSprite;

    sf::Texture borderTex;
    sf::Sprite  borderGreen;
    sf::Sprite  borderBlue;
    sf::Sprite  borderPink;

    const sf::Font& font;

    Slider musicSlider;
    Slider sfxSlider;

    Button btnThemeGreen;
    Button btnThemeBlue;
    Button btnThemePink;
    Button btnClose;

    bool active;

    // Theme đang được áp dụng thực sự (dùng để vẽ border + load asset)
    ThemeType currentTheme;

    // Theme người dùng vừa chọn (chưa apply)
    ThemeType requestedTheme;
    bool themeChangeRequested;          // cờ báo cho main biết cần chạy loading

    float windowWidth;
    float windowHeight;

public:
    SettingsPopup(const sf::Font& fontRef, float windowWidth, float windowHeight);

    void open();
    void close();
    bool isOpen() const { return active; }

    // Apply theme thật (gọi sau khi loading xong)
    void setTheme(ThemeType theme);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window);

    // --- API mới cho main ---
    bool hasThemeChangeRequest() const { return themeChangeRequested; }
    ThemeType getRequestedTheme() const { return requestedTheme; }
    void clearThemeChangeRequest() { themeChangeRequested = false; }

    ThemeType getSelectedTheme() const { return currentTheme; }

    float getMusicVolume() const { return musicSlider.getValue(); }
    float getSfxVolume()   const { return sfxSlider.getValue(); }
};