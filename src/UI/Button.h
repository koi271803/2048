#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// LỚP BUTTON: QUẢN LÝ NÚT BẤM (HIỂN THỊ, HIỆU ỨNG, SỰ KIỆN CLICK)
class Button {
private:
    sf::Texture texture;   // Chứa dữ liệu hình ảnh của nút
    sf::Sprite sprite;     // Khung hiển thị hình ảnh nút lên màn hình
    sf::Text text;         // Đối tượng văn bản (nếu nút có chữ)

    bool isHovered;        
    std::string label;     // Nội dung chữ của nút

    sf::Clock hoverClock;  // Đồng hồ đếm thời gian để làm hiệu ứng 

public:
    // Hàm khởi tạo: Bắt buộc truyền đường dẫn ảnh, Font chữ, nội dung chữ và tọa độ (X, Y)
    Button(const std::string& imagePath, const sf::Font& fontRef, const std::string& btnText, float x, float y);

    void reloadTexture(const std::string& newImagePath);

    // Hàm cập nhật trạng thái mỗi khung hình (xử lý hiệu ứng khi rê chuột)
    void update(const sf::Vector2f& mousePos);

    // Hàm kiểm tra xem người dùng có click chuột trái vào nút không
    bool isClicked(const sf::Event& event, const sf::Vector2f& mousePos) const;

    // Hàm vẽ nút bấm ra cửa sổ game
    void render(sf::RenderWindow& window) const;

    // Hàm lấy nội dung chữ của nút
    std::string getText() const;
};