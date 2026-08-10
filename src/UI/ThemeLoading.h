#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Settings.h"   

class ThemeLoading
{
private:
    // Background
    sf::Texture     bgTexture;
    sf::Sprite      bgSprite;

    // Track (vien) + Fill
    sf::Texture     trackTexture;
    sf::Sprite      trackSprite;
    sf::Texture     fillTexture;
    sf::Sprite      fillSprite;

    // Fallback khi khong load duoc texture
    sf::RectangleShape dimOverlay;
    sf::RectangleShape trackShape;
    sf::RectangleShape fillShape;

    // Text
    sf::Text        loadingText;

    // Progress
    float           progress;
    float           duration;
    float           elapsed;
    bool            active;
    bool            finished;

    float           windowWidth;
    float           windowHeight;
    ThemeType       currentTheme;
    const sf::Font& font;

    // Load texture theo theme
    void loadAssets(ThemeType theme);

    // Cap nhat chieu rong / scale cua fill theo progress
    void updateFillVisual();

public:
    ThemeLoading(const sf::Font& fontRef, float w, float h);

    // Bat man loading voi theme moi
    void start(ThemeType newTheme, float durationSec = 1.2f);

    bool isActive()   const { return active; }
    bool isFinished() const { return finished; }

    void update(float dt);
    void render(sf::RenderWindow& window);
};