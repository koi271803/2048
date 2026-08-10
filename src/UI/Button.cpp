#include "Button.h"
#include <cmath>        
#include <iostream>     

// HÀM KHỞI TẠO NÚT BẤM (Thiết lập hình ảnh, font chữ và tọa độ)
Button::Button(const std::string& imagePath, const sf::Font& fontRef, const std::string& btnText, float x, float y)
    : texture(),
    sprite(texture),
    text(fontRef, btnText, 28),
    isHovered(false),
    label(btnText)
{
    // Nạp ảnh lần đầu tiên
    reloadTexture(imagePath);

    sprite.setPosition(sf::Vector2f(x, y));
     
    // Xử lý text đè lên nút
    if (!btnText.empty()) {
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f));
        text.setPosition(sf::Vector2f(x, y - 3.f));
    }
}

// HÀM NẠP LẠI ẢNH MỚI (DÙNG KHI ĐỔI THEME HOẶC KHỞI TẠO)
void Button::reloadTexture(const std::string& newImagePath) {
    if (texture.loadFromFile(newImagePath)) {
        texture.setSmooth(true);
        sprite.setTexture(texture, true);

        // Canh giữa tâm nút để hiệu ứng phình to được đều 4 hướng
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f));
    }
    else {
        std::cout << "[Loi Button] Khong the nap anh: " << newImagePath << "\n";
    }
}

// HÀM CẬP NHẬT HIỆU ỨNG (Chạy liên tục mỗi khung hình)
void Button::update(const sf::Vector2f& mousePos) {
    if (sprite.getGlobalBounds().contains(mousePos)) {
        if (!isHovered) {
            isHovered = true;
            hoverClock.restart();
        }
        // Hiệu ứng 
        float time = hoverClock.getElapsedTime().asSeconds();
        float scale = 1.0f + 0.04f * std::sin(time * 6.0f);

        sprite.setScale(sf::Vector2f(scale, scale));
        text.setScale(sf::Vector2f(scale, scale));
        sprite.setColor(sf::Color(210, 210, 210));
    }
    else {
        if (isHovered) {
            isHovered = false;
            sprite.setScale(sf::Vector2f(1.0f, 1.0f));
            text.setScale(sf::Vector2f(1.0f, 1.0f));
            sprite.setColor(sf::Color::White);
        }
    }
}

// HÀM KIỂM TRA SỰ KIỆN CLICK CHUỘT
bool Button::isClicked(const sf::Event& event, const sf::Vector2f& mousePos) const {
    if (sprite.getGlobalBounds().contains(mousePos) && event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            return true;
        }
    }
    return false;
}

// HÀM VẼ NÚT BẤM
void Button::render(sf::RenderWindow& window) const {
    window.draw(sprite);
    if (!label.empty()) window.draw(text);
}

std::string Button::getText() const {
    return label;
}