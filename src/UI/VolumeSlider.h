#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <string>

// Lớp quản lý thanh kéo âm lượng (Volume Slider) tương tác bằng chuột
class Slider {
private:
    // CÁC TEXTURE PNG CỦA SLIDER
    sf::Texture trackTexture;        // PNG thanh nền
    sf::Texture fillTexture;         // PNG phần thanh âm lượng đã kéo
	sf::Texture pickerBorderTexture; // PNG viền nút kéo
    sf::Texture pickerTexture;       // PNG nút kéo tròn/hình vuông
    sf::Texture whiteButtonTexture;  // PNG nút trắng phụ (dự phòng)

    // CÁC SPRITE HIỂN THỊ SLIDER
    sf::Sprite track;        // Thanh nền
    sf::Sprite fill;         // Phần thanh đã kéo
	sf::Sprite pickerBorder; // Viền nút kéo
    sf::Sprite picker;       // Nút kéo
    sf::Sprite whiteButton;  // Nút phụ

    // GIÁ TRỊ SLIDER
    float minValue;       // Giá trị nhỏ nhất (0.0f)
    float maxValue;       // Giá trị lớn nhất (100.0f)
    float currentValue;   // Giá trị âm lượng hiện tại (0 -> 100)
    bool isDragging;      // Kiểm tra người chơi có đang giữ chuột kéo nút hay không

    // TỌA ĐỘ VÀ KÍCH THƯỚC
    float sliderX;
    float sliderY;
    float sliderWidth;

    // Cập nhật vị trí hiển thị của Fill và Picker theo phần trăm âm lượng
    void updateVisual();

public:
    // HÀM KHỞI TẠO SLIDER
    // X, Y: Tọa độ bắt đầu
    // width: Chiều dài thanh kéo
    // initialValue: Âm lượng ban đầu (mặc định 50%)
    Slider(
        float x,
        float y,
        float width,
        float initialValue = 50.0f
    );

    // Đổi Theme màu sắc cho Slider (load lại track, fill, picker theo thư mục theme)
    void reloadTextures(const std::string& folder);

    // Xử lý sự kiện kéo thả chuột - Chuẩn SFML 3.1
    void handleEvent(
        const sf::Event& event,
        sf::RenderWindow& window
    );

    // Cập nhật hiệu ứng Hover phóng to nút kéo
    void update(sf::RenderWindow& window);

    // Vẽ Slider ra màn hình
    void draw(sf::RenderWindow& window);

    // Lấy giá trị âm lượng hiện tại
    float getValue() const {
        return currentValue;
    }

    // Đặt giá trị âm lượng bằng code
    void setValue(float value);
};