#include "LoadingScreen.h"
#include <cmath>
#include <iostream>

LoadingScreen::LoadingScreen(const sf::Font& font, float windowWidth, float windowHeight, const std::string& bgImagePath)
    : bgSprite(bgTexture),
    startText(font, "PRESS TO START...", 35)
{
    // Tải ảnh nền
    if (bgTexture.loadFromFile(bgImagePath)) {
        // Bật tính năng làm mượt ảnh (khử răng cưa) khi Fullscreen
        bgSprite = sf::Sprite(bgTexture);
        // Tự động tính toán tỷ lệ để ảnh co giãn vừa khít mọi màn hình
        float scaleX = windowWidth / static_cast<float>(bgTexture.getSize().x);
        float scaleY = windowHeight / static_cast<float>(bgTexture.getSize().y);
        bgSprite.setScale({ scaleX, scaleY });
    }
    else {
        std::cerr << "Loi: Khong the tai anh background tu duong dan: " << bgImagePath << "!\n";
    }

    // Cài đặt màu sắc và căn giữa cho chữ
    startText.setFillColor(sf::Color::White);

    sf::FloatRect textRect = startText.getLocalBounds();
    startText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
                         textRect.position.y + textRect.size.y / 2.0f });

    // Đặt chữ ở tọa độ 85% chiều cao màn hình
    startText.setPosition({ windowWidth / 2.0f, windowHeight * 0.95f });
}

GameState LoadingScreen::handleEvent(const sf::Event& event) {
    // CHỈ CLICK CHUỘT TRÁI MỚI CHUYỂN SANG MAIN MENU (Bấm phím bàn phím sẽ không có tác dụng)
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            return GameState::MAIN_MENU;
        }
    }
    return GameState::LOADING;
}
void LoadingScreen::update() {
    float elapsedTime = animationClock.getElapsedTime().asSeconds();
    float frequency = 3.0f;   // Tốc độ nhấp nháy
    float amplitude = 77.5f;  // Biên độ mờ
    float baseAlpha = 177.5f; // Độ mờ cơ bản

    // Hiệu ứng mờ dần / tỏ dần mượt mà
    std::uint8_t currentAlpha = static_cast<std::uint8_t>(baseAlpha + amplitude * std::sin(elapsedTime * frequency));
    startText.setFillColor(sf::Color(255, 255, 255, currentAlpha));
}

void LoadingScreen::render(sf::RenderWindow& window) const {
    window.draw(bgSprite);
    window.draw(startText);
}