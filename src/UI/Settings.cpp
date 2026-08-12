#include "Settings.h"
#include <iostream>

SettingsPopup::SettingsPopup(const sf::Font& fontRef, float w, float h)
    : bgTexture()
    , bgSprite(bgTexture)
    , borderTex()
    , borderGreen(borderTex)
    , borderBlue(borderTex)
    , borderPink(borderTex)
    , font(fontRef)
    , windowWidth(w)
    , windowHeight(h)
    , musicSlider(w / 2.f - 150.f, h / 2.f - 100.f, 400.f, 50.f)
    , sfxSlider(w / 2.f - 150.f, h / 2.f + 25.f, 400.f, 50.f)
    , btnThemeGreen("assets/images/green/popups/settings/green_picker.png", fontRef, "",
        w / 2.f - 50.f, h / 2.f + 170.f)
    , btnThemeBlue("assets/images/green/popups/settings/blue_picker.png", fontRef, "",
        w / 2.f + 100.f, h / 2.f + 170.f)
    , btnThemePink("assets/images/green/popups/settings/pink_picker.png", fontRef, "",
        w / 2.f + 250.f, h / 2.f + 170.f)
    , btnClose("assets/images/green/popups/settings/close.png", fontRef, "",
        w / 2.f + 380.f, h / 2.f - 200.f)
    , active(false)
    , currentTheme(THEME_GREEN)
    , requestedTheme(THEME_GREEN)
    , themeChangeRequested(false)
{
    dimOverlay.setSize(sf::Vector2f(w, h));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    bgSprite.setPosition(sf::Vector2f(w / 2.f, h / 2.f));

    // Khởi tạo theme mặc định
    setTheme(THEME_GREEN);
}

// ============================================================
// APPLY THEME THẬT (chỉ gọi sau khi ThemeLoading xong)
// ============================================================
void SettingsPopup::setTheme(ThemeType theme)
{
    currentTheme = theme;
    requestedTheme = theme;          // đồng bộ luôn

    std::string folder = getThemeFolder(theme);

    // Background
    std::string bgPath = "assets/images/" + folder + "/popups/settings/bg.png";
    if (bgTexture.loadFromFile(bgPath)) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture, true);

        sf::FloatRect b = bgSprite.getLocalBounds();
        bgSprite.setOrigin(sf::Vector2f(
            b.position.x + b.size.x / 2.f,
            b.position.y + b.size.y / 2.f));
    }

    // Border (viền highlight)
    std::string borderPath = "assets/images/" + folder + "/popups/settings/picker_border.png";
    if (borderTex.loadFromFile(borderPath)) {
        borderTex.setSmooth(true);
        borderGreen.setTexture(borderTex, true);
        borderBlue.setTexture(borderTex, true);
        borderPink.setTexture(borderTex, true);

        sf::FloatRect b = borderGreen.getLocalBounds();
        sf::Vector2f origin(b.position.x + b.size.x / 2.f,
            b.position.y + b.size.y / 2.f);

        borderGreen.setOrigin(origin);
        borderBlue.setOrigin(origin);
        borderPink.setOrigin(origin);

        borderGreen.setPosition(sf::Vector2f(windowWidth / 2.f - 50.f, windowHeight / 2.f + 170.f));
        borderBlue.setPosition(sf::Vector2f(windowWidth / 2.f + 100.f, windowHeight / 2.f + 170.f));
        borderPink.setPosition(sf::Vector2f(windowWidth / 2.f + 250.f, windowHeight / 2.f + 170.f));
    }

    // Nút theme + nút close
    btnThemeGreen.reloadTexture("assets/images/" + folder + "/popups/settings/green_picker.png");
    btnThemeBlue.reloadTexture("assets/images/" + folder + "/popups/settings/blue_picker.png");
    btnThemePink.reloadTexture("assets/images/" + folder + "/popups/settings/pink_picker.png");
    btnClose.reloadTexture("assets/images/" + folder + "/popups/settings/close.png");

    // Slider
    musicSlider.reloadTextures(folder);
    sfxSlider.reloadTextures(folder);
}

void SettingsPopup::open() { active = true; }
void SettingsPopup::close() { active = false; }

// ============================================================
// XỬ LÝ SỰ KIỆN
// ============================================================
void SettingsPopup::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (!active) return;

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    musicSlider.handleEvent(event, window);
    sfxSlider.handleEvent(event, window);

    // --- Click theme → chỉ đánh dấu, KHÔNG gọi setTheme ngay ---
    if (btnThemeGreen.isClicked(event, mousePos) && currentTheme != THEME_GREEN) {
        requestedTheme = THEME_GREEN;
        themeChangeRequested = true;
    }
    if (btnThemeBlue.isClicked(event, mousePos) && currentTheme != THEME_BLUE) {
        requestedTheme = THEME_BLUE;
        themeChangeRequested = true;
    }
    if (btnThemePink.isClicked(event, mousePos) && currentTheme != THEME_PINK) {
        requestedTheme = THEME_PINK;
        themeChangeRequested = true;
    }

    // Nút đóng
    if (btnClose.isClicked(event, mousePos))
        close();
}

// ============================================================
// UPDATE
// ============================================================
void SettingsPopup::update(sf::RenderWindow& window)
{
    if (!active) return;

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    musicSlider.update(window);
    sfxSlider.update(window);

    btnThemeGreen.update(mousePos);
    btnThemeBlue.update(mousePos);
    btnThemePink.update(mousePos);
    btnClose.update(mousePos);
}

// ============================================================
// RENDER
// ============================================================
void SettingsPopup::render(sf::RenderWindow& window)
{
    if (!active) return;

    window.draw(dimOverlay);
    window.draw(bgSprite);

    musicSlider.draw(window);
    sfxSlider.draw(window);

    // Vẽ border theo theme đang được chọn (requested hoặc current)
    ThemeType showTheme = themeChangeRequested ? requestedTheme : currentTheme;

    if (showTheme == THEME_GREEN) window.draw(borderGreen);
    else if (showTheme == THEME_BLUE) window.draw(borderBlue);
    else if (showTheme == THEME_PINK) window.draw(borderPink);

    btnThemeGreen.render(window);
    btnThemeBlue.render(window);
    btnThemePink.render(window);
    btnClose.render(window);
}