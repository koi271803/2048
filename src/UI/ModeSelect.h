#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "Settings.h" 

// Các tùy chọn trong màn hình chọn Mode
enum class ModeOption {
    NONE,
    MODE_4x4,
    MODE_5x5,
    MODE_6x6,
    BACK
};

class ModeSelect {
private:
    const sf::Font& font;
    ThemeType currentTheme;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

	sf::RectangleShape dimOverlay; // Lớp phủ mờ đen phía sau khi mở popup
    // Các nút chọn kích thước bàn cờ và nút quay lại
    Button btn4x4;
    Button btn5x5;
    Button btn6x6;
    Button btnBack;
public:
    ModeSelect(const sf::Font& fontRef);

    void setTheme(ThemeType theme);

    ModeOption handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window) const;
};