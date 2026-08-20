#include "ThemeLoading.h"
#include <iostream>
#include <algorithm>

// CONSTRUCTOR
ThemeLoading::ThemeLoading(const sf::Font& fontRef, float w, float h)
    : font(fontRef)
    , windowWidth(w)
    , windowHeight(h)
    , bgSprite(bgTexture)         
    , trackSprite(trackTexture)
    , fillSprite(fillTexture)
    , loadingText(fontRef, "LOADING...", 96)
    , progress(0.f)
    , duration(1.2f)
    , elapsed(0.f)
    , active(false)
    , finished(false)
    , currentTheme(THEME_GREEN)
{
    // Fallback dim
    dimOverlay.setSize(sf::Vector2f(w, h));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 100));

    // Fallback track (hinh chu nhat)
    trackShape.setSize(sf::Vector2f(420.f, 28.f));
    trackShape.setFillColor(sf::Color(255, 255, 255, 230));
    trackShape.setOrigin(sf::Vector2f(210.f, 14.f));
    trackShape.setPosition(sf::Vector2f(w / 2.f, h / 2.f + 50.f));

    // Fallback fill
    fillShape.setSize(sf::Vector2f(0.f, 20.f));
    fillShape.setOrigin(sf::Vector2f(0.f, 10.f));
    fillShape.setPosition(sf::Vector2f(w / 2.f - 200.f, h / 2.f + 50.f));

    // Text
    loadingText.setStyle(sf::Text::Bold);
    loadingText.setFillColor(sf::Color(28, 77, 46));

    sf::FloatRect tb = loadingText.getLocalBounds();
    loadingText.setOrigin(sf::Vector2f(
        tb.position.x + tb.size.x / 2.f,
        tb.position.y + tb.size.y / 2.f));
    loadingText.setPosition(sf::Vector2f(w / 2.f, h / 2.f + 50.f));
}

void ThemeLoading::loadAssets(ThemeType theme)
{
    currentTheme = theme;
    std::string folder = getThemeFolder(theme);

    // Background
    std::string bgPath = "assets/images/" + folder + "/loading/loading_bg.png";
    if (bgTexture.loadFromFile(bgPath))
    {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture, true);

        float sx = windowWidth / static_cast<float>(bgTexture.getSize().x);
        float sy = windowHeight / static_cast<float>(bgTexture.getSize().y);
        bgSprite.setScale(sf::Vector2f(sx, sy));
        bgSprite.setPosition(sf::Vector2f(0.f, 0.f));
    }
    else
    {
        std::cout << "[ThemeLoading] Khong load duoc bg: " << bgPath << "\n";
    }

    // Track
    std::string trackPath = "assets/images/" + folder + "/loading/track.png";
    if (trackTexture.loadFromFile(trackPath))
    {
        trackTexture.setSmooth(true);
        trackSprite.setTexture(trackTexture, true);

        // Scale 1.2
        trackSprite.setScale(sf::Vector2f(0.75f, 0.75f));

        sf::FloatRect b = trackSprite.getLocalBounds();
        trackSprite.setOrigin(sf::Vector2f(
            b.position.x + b.size.x / 2.f,
            b.position.y + b.size.y / 2.f));

        trackSprite.setPosition(sf::Vector2f(windowWidth / 2.f, windowHeight / 2.f + 50.f));
    }
    else
    {
        std::cout << "[ThemeLoading] Khong load duoc track: " << trackPath << "\n";
    }

    // Fill 
    std::string fillPath = "assets/images/" + folder + "/loading/fill.png";
    if (fillTexture.loadFromFile(fillPath))
    {
        fillTexture.setSmooth(true);
        fillSprite.setTexture(fillTexture, true);

        // Origin bên TRÁI + giữa theo chiều dọc
        sf::FloatRect b = fillSprite.getLocalBounds();
        fillSprite.setOrigin(sf::Vector2f(
            b.position.x,
            b.position.y + b.size.y / 2.f));

        // Vị trí bắt đầu = mép trái track + padding lớn hơn để rìa track dày
        float trackW = trackSprite.getGlobalBounds().size.x;

        // Padding lớn → rìa track nhìn dày hơn
        float paddingX = 14.f;   // tăng padding để rìa dày

        fillSprite.setPosition(sf::Vector2f(
            windowWidth / 2.f - trackW / 2.f + paddingX,
            windowHeight / 2.f + 50.f));
    }
    else
    {
        std::cout << "[ThemeLoading] Khong load duoc fill: " << fillPath << "\n";
    }

    // Text 
    if (theme == THEME_BLUE)
        loadingText.setFillColor(sf::Color(30, 90, 160));
    else if (theme == THEME_PINK)
        loadingText.setFillColor(sf::Color(180, 60, 120));
    else
        loadingText.setFillColor(sf::Color(28, 77, 46));

    sf::FloatRect tb = loadingText.getLocalBounds();
    loadingText.setOrigin(sf::Vector2f(
        tb.position.x,                          
        tb.position.y + tb.size.y / 2.f));

    // Đặt chữ lệch về bên trái của thanh loading
    loadingText.setPosition(sf::Vector2f(
        windowWidth / 2.f - 280.f,              // ← chỉnh số này để dịch trái/phải
        windowHeight / 2.f - 55.f               // ← chỉnh số này để dịch lên/xuống
    ));
}
// CAP NHAT HINH ANH FILL THEO PROGRESS
void ThemeLoading::updateFillVisual()
{
    if (fillTexture.getSize().x == 0 || trackTexture.getSize().x == 0)
        return;

    float trackW = trackSprite.getGlobalBounds().size.x;
    float trackH = trackSprite.getGlobalBounds().size.y;

    // Padding lớn hơn → rìa track dày rõ
    float paddingX = 14.f * 2.f;   // *2 vì scale 1.5
    float paddingY = 8.f * 2.f;

    float maxFillWidth = trackW - paddingX;

    // Scale X theo progress (từ trái → phải)
    float nativeW = static_cast<float>(fillTexture.getSize().x);
    if (nativeW < 1.f) nativeW = 1.f;

    float targetScaleX = (maxFillWidth * progress) / nativeW;

    // Scale Y vừa khít chiều cao track (trừ padding)
    float nativeH = static_cast<float>(fillTexture.getSize().y);
    float targetScaleY = (trackH - paddingY) / nativeH;

    fillSprite.setScale(sf::Vector2f(
        std::max(0.f, targetScaleX),
        std::max(0.f, targetScaleY)
    ));
}
// BAT LOADING
void ThemeLoading::start(ThemeType newTheme, float durationSec)
{
    duration = std::max(0.3f, durationSec);
    elapsed = 0.f;
    progress = 0.f;
    active = true;
    finished = false;

    loadAssets(newTheme);
    updateFillVisual();
}

// UPDATE
void ThemeLoading::update(float dt)
{
    if (!active || finished)
        return;

    elapsed += dt;
    progress = std::clamp(elapsed / duration, 0.f, 1.f);

    updateFillVisual();

    if (progress >= 1.f)
    {
        finished = true;
        active = false;
    }
}

// RENDER
void ThemeLoading::render(sf::RenderWindow& window)
{
    // Van ve duoc frame cuoi cung khi vua finished
    if (!active && !finished)
        return;

    // Background
    if (bgTexture.getSize().x > 0)
        window.draw(bgSprite);
    else
        window.draw(dimOverlay);

    // Text
    window.draw(loadingText);

    // Track + Fill
    if (trackTexture.getSize().x > 0)
    {
        window.draw(trackSprite);

        if (fillTexture.getSize().x > 0 && progress > 0.001f)
            window.draw(fillSprite);
    }
    else
    {
        // Fallback hinh chu nhat
        window.draw(trackShape);
        if (progress > 0.001f)
            window.draw(fillShape);
    }
}