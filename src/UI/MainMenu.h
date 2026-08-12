#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Settings.h" 

// DANH SÁCH LỰA CHỌN MENU
enum class MenuOption {
    NONE,
    OPT_NEW_GAME,
    OPT_CHALLENGE,
    OPT_HOW_TO_PLAY,
    OPT_SETTINGS
};

// LỚP MAIN MENU: QUẢN LÝ MÀN HÌNH CHÍNH

class MainMenu {
private:
    const sf::Font& font;

    ThemeType currentTheme;
    ThemeType getSelectedTheme() const { return currentTheme; }
    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    // 4 đối tượng nút bấm
    Button btnNewGame;
    Button btnChallenge;
    Button btnHowToPlay;
    Button btnSettings;

public:
    MainMenu(const sf::Font& fontRef);

    void setTheme(ThemeType theme);

    MenuOption handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window) const;
};