#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"
#include "GameState.h"
#include "Settings.h" 

// HOW TO PLAY: QUẢN LÝ MÀN HÌNH HƯỚNG DẪN 3 SLIDES
class HowToPlay {
private:
    const sf::Font& font;   // Tham chiếu Font chữ gốc của hệ thống

    ThemeType currentTheme; // Lưu trữ Theme hiện tại (GREEN, BLUE, PINK)

    // Danh sách 3 Texture và 3 Sprite đại diện cho 3 slide hướng dẫn
    std::vector<sf::Texture> slideTextures;
    std::vector<sf::Sprite> slideSprites;

    // Mảng lưu tọa độ X hiện tại của từng slide để làm hiệu ứng trượt mượt mà
    std::vector<float> slideX;

    int currentSlide;   // Chỉ số slide hiện tại (0: Slide 1, 1: Slide 2, 2: Slide 3)
    float windowWidth;  // Chiều rộng cửa sổ game
    float windowHeight; // Chiều cao cửa sổ game

    // Các nút bấm điều hướng
    Button btnNext;  // Nút Next (hoặc Done ở slide cuối)
    Button btnPrev;  // Nút Prev (Quay lại slide trước)

public:
    // Hàm khởi tạo màn hình How To Play
    HowToPlay(const sf::Font& fontRef, float screenWidth, float screenHeight);

    // Cập nhật Theme mới cho toàn bộ slide và các nút bấm
    void setTheme(ThemeType theme);

    // Reset các slide về trang đầu tiên mỗi khi người chơi mở lên
    void reset();

    // Bắt và xử lý sự kiện click chuột
    GameState handleEvent(const sf::Event& event, sf::RenderWindow& window);

    // Cập nhật hiệu ứng Hover nút và hiệu ứng trượt trang mượt mà (Lerp)
    void update(sf::RenderWindow& window);

    // Vẽ giao diện ra màn hình
    void render(sf::RenderWindow& window) const;
};