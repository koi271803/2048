#include "MainMenu.h"
#include <iostream>

// HÀM KHỞI TẠO (Nạp nền và nạp 4 nút bấm)
MainMenu::MainMenu(const sf::Font& fontRef)
    : font(fontRef),
    currentTheme(THEME_GREEN),

    bgTexture(),
    bgSprite(bgTexture),

    btnNewGame("assets/images/green/buttons/new_game.png", fontRef, "", 530.0f, 400.0f),
    btnChallenge("assets/images/green/buttons/challenge.png", fontRef, "", 530.0f, 570.0f),
    btnHowToPlay("assets/images/green/buttons/how_to_play.png", fontRef, "", 530.0f, 740.0f),
    btnSettings("assets/images/green/buttons/settings.png", fontRef, "", 530.0f, 910.0f)
{
    // Gọi hàm set theme ngay lúc khởi tạo để đồng bộ toàn bộ ảnh
    setTheme(THEME_GREEN);
}

// HÀM ĐỔI THEME DỘNG CHO MÀN HÌNH CHÍNH
void MainMenu::setTheme(ThemeType theme) {
    currentTheme = theme;
    std::string folder = getThemeFolder(theme);

    // 1. Nạp lại ảnh nền theo Theme mới
    std::string bgPath = "assets/images/" + folder + "/backgrounds/main_menu/bg.png";
    (void)bgTexture.loadFromFile(bgPath);
    bgTexture.setSmooth(true);
    bgSprite.setTexture(bgTexture, true);

    // 2. Ép các nút nạp lại hình ảnh theo thư mục màu tương ứng
    btnNewGame.reloadTexture("assets/images/" + folder + "/buttons/new_game.png");
    btnChallenge.reloadTexture("assets/images/" + folder + "/buttons/challenge.png");
    btnHowToPlay.reloadTexture("assets/images/" + folder + "/buttons/how_to_play.png");
    btnSettings.reloadTexture("assets/images/" + folder + "/buttons/settings.png");
}
// HÀM XỬ LÝ SỰ KIỆN BẤM CHUỘT
MenuOption MainMenu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Gọi đúng kiểu MenuOption::OPT_...
    if (btnNewGame.isClicked(event, mousePos))   return MenuOption::OPT_NEW_GAME;
    if (btnChallenge.isClicked(event, mousePos)) return MenuOption::OPT_CHALLENGE;
    if (btnHowToPlay.isClicked(event, mousePos)) return MenuOption::OPT_HOW_TO_PLAY;
    if (btnSettings.isClicked(event, mousePos))  return MenuOption::OPT_SETTINGS;

    return MenuOption::NONE;
}

// HÀM CẬP NHẬT HOẠT ẢNH MỖI KHUNG HÌNH (Xử lý Hover)
void MainMenu::update(sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    btnNewGame.update(mousePos);
    btnChallenge.update(mousePos);
    btnHowToPlay.update(mousePos);
    btnSettings.update(mousePos);
}
// HÀM VẼ GIAO DIỆN RA MÀN HÌNH
void MainMenu::render(sf::RenderWindow& window) const {
    window.draw(bgSprite);

    btnNewGame.render(window);
    btnChallenge.render(window);
    btnHowToPlay.render(window);
    btnSettings.render(window);
}