#include "VolumeSlider.h"
#include <iostream>

// HÀM KHỞI TẠO SLIDER
Slider::Slider(
    float x,
    float y,
    float width,
    float initialValue)
    : trackTexture(), track(trackTexture),
    fillTexture(), fill(fillTexture),
    pickerBorderTexture(), pickerBorder(pickerBorderTexture),
    pickerTexture(), picker(pickerTexture),
    whiteButtonTexture(), whiteButton(whiteButtonTexture)
{
    // 1. Cài đặt dải giá trị
    minValue = 0.0f;
    maxValue = 100.0f;

    // Giữ giá trị ban đầu trong khoảng 0 -> 100
    currentValue = std::clamp(initialValue, minValue, maxValue);
    isDragging = false;

    // 2. Lưu lại tọa độ và độ rộng thanh kéo
    sliderX = x;
    sliderY = y;
    sliderWidth = width;

    // 3. Mặc định nạp giao diện theme xanh lá (green)
    reloadTextures("green");
}

// HÀM ĐỔI THEME DỘNG CHO SLIDER (Thanh kéo đổi màu xuyệt tông)

void Slider::reloadTextures(const std::string& folder) {
    // Đường dẫn gốc dùng chung
    std::string basePath = "assets/images/" + folder + "/popups/settings/";

    //  Load ảnh Thanh nền (Track) - ĐÃ BỎ LỆNH DỰ PHÒNG
    (void)trackTexture.loadFromFile(basePath + "track.png");
    trackTexture.setSmooth(true);
    track.setTexture(trackTexture, true);

    // Load ảnh Thanh giá trị (Fill)
    (void)fillTexture.loadFromFile(basePath + "fill.png");
    fillTexture.setSmooth(true);
    fill.setTexture(fillTexture, true);

    // Load ảnh Khung viền của Picker (Cần chuẩn bị file picker_border.png)
    (void)pickerBorderTexture.loadFromFile(basePath + "picker_border.png");
    pickerBorderTexture.setSmooth(true);
    pickerBorder.setTexture(pickerBorderTexture, true);

    // Load ảnh Nút kéo (Picker lõi bên trong)
    if (trackTexture.loadFromFile(basePath + "track.png")) {
        trackTexture.setSmooth(true);
        track.setTexture(trackTexture, true);
    }

    // Co giãn thanh Track theo độ rộng sliderWidth (Chuẩn SFML 3.1)
    sf::FloatRect trackBounds = track.getLocalBounds();
    if (trackBounds.size.x > 1.2f) {
        float scaleX = sliderWidth / trackBounds.size.x;
        track.setScale(sf::Vector2f(scaleX, 1.05f));
    }
    track.setPosition(sf::Vector2f(sliderX - 10.0f, sliderY - 7.0f));

    // Canh giữa tâm Origin cho Nút kéo (Picker)
    sf::FloatRect pickerBounds = picker.getLocalBounds();
    picker.setOrigin(sf::Vector2f(
        pickerBounds.position.x + pickerBounds.size.x / 1.5f,
        pickerBounds.position.y + pickerBounds.size.y / 1.0f
    ));

    // Canh giữa tâm Origin cho Khung viền (Border)
    sf::FloatRect borderBounds = pickerBorder.getLocalBounds();
    pickerBorder.setOrigin(sf::Vector2f(
        borderBounds.position.x + borderBounds.size.x / 2.0f,
        borderBounds.position.y + borderBounds.size.y / 2.0f
    ));

    // Cập nhật vị trí hiển thị của Fill và Picker theo giá trị hiện tại
    updateVisual();
}

// CẬP NHẬT VỊ TRÍ PHẦN FILL VÀ NÚT PICKER
void Slider::updateVisual() {
    // Tính phần trăm âm lượng (từ 0.0 đến 1.0)
    float percent = (currentValue - minValue) / (maxValue - minValue);

    // Tọa độ X hiện tại của nút kéo
    float pickerX = sliderX + sliderWidth * percent;

    // Cập nhật độ dài phần Fill đã kéo
    sf::FloatRect fillBounds = fill.getLocalBounds();
    if (fillBounds.size.x > 0.0f) {
        float scaleX = (sliderWidth * percent) / fillBounds.size.x;
        fill.setScale(sf::Vector2f(scaleX, 1.0f));
    }
    fill.setPosition(sf::Vector2f(sliderX, sliderY));

    // Cập nhật vị trí Nút kéo Picker VÀ Khung viền (Đi cùng nhau)
    float pickerY = sliderY + (track.getGlobalBounds().size.y / 2.0f);
    float offsetY = -5.0f;
    picker.setPosition(sf::Vector2f(pickerX, pickerY + offsetY));
    pickerBorder.setPosition(sf::Vector2f(pickerX, pickerY + offsetY)); // Đi theo picker
}

// XỬ LÝ SỰ KIỆN KÉO THẢ CHUỘT - SFML 3.1
void Slider::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // BẤM CHUỘT TRÁI 
    if (const auto* mousePress = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePress->button == sf::Mouse::Button::Left) {
            // Nếu click trúng Picker (hoặc Viền của nó) hoặc trực tiếp lên thanh Track
            if (pickerBorder.getGlobalBounds().contains(mousePos) ||
                picker.getGlobalBounds().contains(mousePos) ||
                track.getGlobalBounds().contains(mousePos))
            {
                isDragging = true;

                // Tính toán giá trị mới từ vị trí con trỏ chuột
                float newX = std::clamp(mousePos.x, sliderX, sliderX + sliderWidth);
                currentValue = ((newX - sliderX) / sliderWidth) * 100.0f;

                updateVisual();
            }
        }
    }

    // THẢ CHUỘT 
    if (const auto* mouseRelease = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseRelease->button == sf::Mouse::Button::Left) {
            isDragging = false;
        }
    }

    // KÉO CHUỘT 
    if (isDragging && event.is<sf::Event::MouseMoved>()) {
        float newX = std::clamp(mousePos.x, sliderX, sliderX + sliderWidth);
        currentValue = ((newX - sliderX) / sliderWidth) * 100.0f;

        updateVisual();
    }
}
// CẬP NHẬT HIỆU ỨNG HOVER NẢY PHÌNH TO NÚT KÉO
void Slider::update(sf::RenderWindow& window) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Khi di chuột vào nút kéo (hoặc đang giữ kéo chuột)
    if (pickerBorder.getGlobalBounds().contains(mousePos) || picker.getGlobalBounds().contains(mousePos) || isDragging) {
        picker.setScale(sf::Vector2f(1.0f, 1.0f));       
        pickerBorder.setScale(sf::Vector2f(1.0f, 1.0f)); 
    }
    else {
        picker.setScale(sf::Vector2f(1.0f, 1.0f));         
        pickerBorder.setScale(sf::Vector2f(1.0f, 1.0f));   
    }
}

// VẼ SLIDER RA CỬA SỔ GAME
void Slider::draw(sf::RenderWindow& window) {
    window.draw(track);         // Lớp 1: Thanh nền
    window.draw(fill);          // Lớp 2: Thanh giá trị đã kéo
    window.draw(pickerBorder);  // Lớp 3: Khung viền (vẽ trước để lót bên dưới)
    window.draw(picker);        // Lớp 4: Lõi màu bên trong của nút kéo
}

// Set giá trị Slider bằng code
void Slider::setValue(float value) {
    currentValue = std::clamp(value, minValue, maxValue);
    updateVisual();
}