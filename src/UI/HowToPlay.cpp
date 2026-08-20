#include "HowToPlay.h"
#include <iostream>

// HÀM KHỞI TẠO MÀN HÌNH HOW TO PLAY
HowToPlay::HowToPlay(const sf::Font& fontRef, float screenWidth, float screenHeight)
    : font(fontRef),
    currentTheme(THEME_GREEN),
    currentSlide(0),
    windowWidth(screenWidth),
    windowHeight(screenHeight),
    btnNext("assets/images/green/buttons/next.png", fontRef, "", screenWidth - 150.0f, screenHeight - 100.0f),
    btnPrev("assets/images/green/buttons/prev.png", fontRef, "", 150.0f, screenHeight - 100.0f)
{
    slideTextures.resize(3);
    slideX.resize(3);

    // 2. Nạp dữ liệu
    for (int i = 0; i < 3; ++i) {
        std::string path = "assets/images/green/backgrounds/rule/slide_" + std::to_string(i + 1) + ".png";

        if (!slideTextures[i].loadFromFile(path)) {
            std::cout << "[Lỗi HowToPlay] Khong tim thay: " << path << "\n";
        }
        slideTextures[i].setSmooth(true);

        slideSprites.emplace_back(slideTextures[i]);

        // Căn giữa tâm cho ảnh slide
        sf::FloatRect bounds = slideSprites[i].getLocalBounds();
        slideSprites[i].setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f));
        slideSprites[i].setPosition(sf::Vector2f(0.0f, screenHeight / 2.0f));
    }

    // Nạp Theme xanh lá mặc định ban đầu
    setTheme(THEME_GREEN);

    // Đặt vị trí xuất phát cho các slide
    reset();
}

// HÀM ĐỔI THEME CHO MÀN HÌNH HOW TO PLAY

void HowToPlay::setTheme(ThemeType theme) {
    currentTheme = theme;

    // Lấy tên folder tương ứng ("green", "blue", "pink")
    std::string folder = getThemeFolder(theme);

    // 1. Nạp lại 3 ảnh Slide theo thư mục Theme mới
    for (int i = 0; i < 3; ++i) {
        std::string path = "assets/images/" + folder + "/backgrounds/rule/slide_" + std::to_string(i + 1) + ".png";

        if (slideTextures[i].loadFromFile(path)) {
            slideTextures[i].setSmooth(true);
            // Ép Sprite cập nhật lại Texture vừa nạp
            slideSprites[i].setTexture(slideTextures[i], true);

            // Căn lại tâm Origin phòng trường hợp kích thước ảnh slide khác nhau
            sf::FloatRect bounds = slideSprites[i].getLocalBounds();
            slideSprites[i].setOrigin(sf::Vector2f(
                bounds.position.x + bounds.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f));
        }
    }

    btnNext.reloadTexture("assets/images/" + folder + "/buttons/next.png");
    btnPrev.reloadTexture("assets/images/" + folder + "/buttons/prev.png");
}

// HÀM RESET MÀN HÌNH VỀ TỪ ĐẦU (SLIDE 1)
void HowToPlay::reset() {
    currentSlide = 0; // Đưa về Slide 1

    // Tính toán tọa độ xuất phát cho 3 slide:
    // Slide 1 (i = 0): Nằm ở giữa màn hình
    // Slide 2 (i = 1): Nằm chờ bên phải màn hình
    // Slide 3 (i = 2): Nằm chờ xa hơn ở bên phải
    for (int i = 0; i < 3; ++i) {
        slideX[i] = (windowWidth / 2.0f) + (i * windowWidth);

        sf::Vector2f currentPos = slideSprites[i].getPosition();
        slideSprites[i].setPosition(sf::Vector2f(slideX[i], currentPos.y));
    }
}

// HÀM XỬ LÝ SỰ KIỆN BẤM CHUỘT
GameState HowToPlay::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Nút Next
    if (btnNext.isClicked(event, mousePos)) {
        if (currentSlide < 2) currentSlide++;
        else return GameState::MAIN_MENU;   // Slide cuối → về Main Menu
    }

    // Nút Prev
    if (currentSlide > 0 && btnPrev.isClicked(event, mousePos)) {
        currentSlide--;
    }

    // Phím mũi tên trái / phải
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (key->code == sf::Keyboard::Key::Right || key->code == sf::Keyboard::Key::D) {
            if (currentSlide < 2) currentSlide++;
            else return GameState::MAIN_MENU;
        }
        else if ((key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::A) && currentSlide > 0) {
            currentSlide--;
        }
        else if (key->code == sf::Keyboard::Key::Escape) {
            return GameState::MAIN_MENU;
        }
    }

    return GameState::HOW_TO_PLAY;
}

// HÀM CẬP NHẬT HOẠT ẢNH MỖI KHUNG HÌNH (HOVER VÀ TRƯỢT MƯỢT MÀ)
void HowToPlay::update(sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    btnNext.update(mousePos);
    if (currentSlide > 0) btnPrev.update(mousePos);

    // Hiệu ứng trượt Lerp
    float lerpSpeed = 0.15f;
    for (int i = 0; i < 3; ++i) {
        float targetX = (windowWidth / 2.0f) + ((i - currentSlide) * windowWidth);
        slideX[i] += (targetX - slideX[i]) * lerpSpeed;
        sf::Vector2f currentPos = slideSprites[i].getPosition();
        slideSprites[i].setPosition(sf::Vector2f(slideX[i], currentPos.y));
    }
}
// HÀM VẼ GIAO DIỆN RA MÀN HÌNH
void HowToPlay::render(sf::RenderWindow& window) const {
    for (int i = 0; i < 3; ++i) {
        window.draw(slideSprites[i]);
    }

    if (currentSlide > 0) btnPrev.render(window);
    btnNext.render(window);
}