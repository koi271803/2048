#include "ModeSelect.h"
#include <iostream>

// CONSTRUCTOR
ModeSelect::ModeSelect(const sf::Font& fontRef)
    : font(fontRef)
    , currentTheme(THEME_GREEN)
    , bgTexture()
    , bgSprite(bgTexture)
    // 3 nút chọn mode + nút Back (bạn tự chỉnh tọa độ)
    , btn4x4("assets/images/green/buttons/4x4.png", fontRef, "", 970.0f, 470.0f)
    , btn5x5("assets/images/green/buttons/5x5.png", fontRef, "", 970.0f, 570.0f)
    , btn6x6("assets/images/green/buttons/6x6.png", fontRef, "", 970.0f, 670.0f)
    , btnBack("assets/images/green/buttons/close.png", fontRef, "", 1250.0f, 350.0f)
{
    // Lớp phủ mờ toàn màn hình
    dimOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    setTheme(THEME_GREEN);
}

// THEME
void ModeSelect::setTheme(ThemeType theme)
{
    currentTheme = theme;
    std::string folder = getThemeFolder(theme);

    // Nạp ảnh nền popup
    std::string bgPath = "assets/images/" + folder + "/popups/mode/mode_bg.png";
    if (bgTexture.loadFromFile(bgPath)) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture, true);

        // Căn giữa popup
        sf::FloatRect bounds = bgSprite.getLocalBounds();
        bgSprite.setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        ));
        bgSprite.setPosition(sf::Vector2f(960.f, 540.f));
    }
    else {
        std::cout << "[Lỗi ModeSelect] Khong tim thay: " << bgPath << "\n";
    }

    // Đổi texture nút
    btn4x4.reloadTexture("assets/images/" + folder + "/buttons/4x4.png");
    btn5x5.reloadTexture("assets/images/" + folder + "/buttons/5x5.png");
    btn6x6.reloadTexture("assets/images/" + folder + "/buttons/6x6.png");
    btnBack.reloadTexture("assets/images/" + folder + "/buttons/close.png");
}

// XỬ LÝ SỰ KIỆN
ModeOption ModeSelect::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (btn4x4.isClicked(event, mousePos)) return ModeOption::MODE_4x4;
    if (btn5x5.isClicked(event, mousePos)) return ModeOption::MODE_5x5;
    if (btn6x6.isClicked(event, mousePos)) return ModeOption::MODE_6x6;
    if (btnBack.isClicked(event, mousePos)) return ModeOption::BACK;

    return ModeOption::NONE;
}

// UPDATE
void ModeSelect::update(sf::RenderWindow& window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    btn4x4.update(mousePos);
    btn5x5.update(mousePos);
    btn6x6.update(mousePos);
    btnBack.update(mousePos);
}

// RENDER
void ModeSelect::render(sf::RenderWindow& window) const
{
    // 1. Làm mờ Main Menu
    window.draw(dimOverlay);

    // 2. Vẽ popup (đã căn giữa)
    window.draw(bgSprite);

    // 3. Vẽ các nút
    btn4x4.render(window);
    btn5x5.render(window);
    btn6x6.render(window);
    btnBack.render(window);
}