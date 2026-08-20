#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GameState.h"

class LoadingScreen {
private:
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    sf::Text startText;

    // Đồng hồ đếm thời gian cho hàm sin (hiệu ứng nhấp nháy)
    sf::Clock animationClock;

public:
    LoadingScreen(const sf::Font& font, float windowWidth, float windowHeight, const std::string& bgImagePath);

    GameState handleEvent(const sf::Event& event);
    void update();
    void render(sf::RenderWindow& window) const;
};