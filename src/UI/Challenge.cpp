#include "Challenge.h"
#include <iostream>


// HÀM KHỞI TẠO
Challenge::Challenge(const sf::Font& fontRef, float width, float height)
    : font(fontRef), windowWidth(width), windowHeight(height),
    currentTheme(THEME_GREEN), isPopupOpen(false), selectedChapter(0),
    highestUnlockedChapter(0),               // Ban đầu chỉ mở Chapter 1

    bgTexture(), bgSprite(bgTexture),
    popupTexture(), popupSprite(popupTexture),
    btnClose("assets/images/green/buttons/close.png", fontRef, "", width - 80.f, 80.f)
{
    dimOverlay.setSize(sf::Vector2f(width, height));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 180));

    // TỌA ĐỘ 5 NÚT CHAPTER (bạn có thể chỉnh lại cho khớp map)
    sf::Vector2f chapterPositions[5] = {
        sf::Vector2f(790.0f, 890.0f),   // Chapter 1
        sf::Vector2f(1100.0f, 670.0f),   // Chapter 2
        sf::Vector2f(1120.0f, 390.0f),   // Chapter 3
        sf::Vector2f(800.0f, 320.0f),  // Chapter 4
        sf::Vector2f(960.0f, 195.0f)   // Chapter 5
    };

    for (int i = 0; i < 5; ++i) {
        std::string stateSuffix = (i == 0) ? "_normal" : "_locked";
        std::string path = "assets/images/green/buttons/chapter" + std::to_string(i + 1) + stateSuffix + ".png";

        Button* btn = new Button(path, fontRef, "", chapterPositions[i].x, chapterPositions[i].y);
        btnChapters.push_back(btn);
    }

    // Nút trong popup
    // Nút Start 
    btnStart = new Button("assets/images/green/buttons/start.png", fontRef, "",
        width / 2.0f, height / 2.0f + 270.f);

    // Nút Close 
    btnClosePopup = new Button("assets/images/green/buttons/close.png", fontRef, "",
        width / 2.0f + 450.f, height / 2.0f - 220.f);

    initChapterData();
    setTheme(THEME_GREEN);
}

Challenge::~Challenge() {
    for (Button* btn : btnChapters) delete btn;
    delete btnStart;
    delete btnClosePopup;
}

// ĐIỀU KIỆN 5 CHAPTER (khớp chính xác với ảnh)
void Challenge::initChapterData() {
    // Chapter 1: 4x4 - Reach 256 - 50 moves - Không có Power
    chapters.push_back({ 256,  200, 4,  0, 0, 0 });

    // Chapter 2: 4x4 - Reach 1024 - 100 moves - Chỉ Undo
    chapters.push_back({ 1024, 500, 4,  2, 0, 0 });

    // Chapter 3: 5x5 - Reach 2048 - 250 moves - Undo + Swap
    chapters.push_back({ 2048, 600, 5,  1, 1, 0 });

    // Chapter 4: 5x5 - Reach 4096 - 500 moves - Undo + Swap
    chapters.push_back({ 4096, 700, 5,  1, 1, 0 });

    // Chapter 5: 6x6 - Reach 8192 - 700 moves - Full Power
    chapters.push_back({ 8192, 900, 6,  1, 1, 1 });
}

// ĐỔI THEME 
void Challenge::setTheme(ThemeType theme) {
    currentTheme = theme;
    std::string folder = getThemeFolder(theme);

    if (bgTexture.loadFromFile("assets/images/" + folder + "/backgrounds/challenge/bg.png")) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture, true);
    }

    btnClose.reloadTexture("assets/images/" + folder + "/buttons/close.png");
    btnStart->reloadTexture("assets/images/" + folder + "/buttons/start.png");
    btnClosePopup->reloadTexture("assets/images/" + folder + "/buttons/close.png");

    // Cập nhật 3 layer: completed / normal / locked
    for (int i = 0; i < 5; ++i) {
        std::string stateSuffix;
        if (i < highestUnlockedChapter) {
            stateSuffix = "_completed";
        }
        else if (i == highestUnlockedChapter) {
            stateSuffix = "_normal";
        }
        else {
            stateSuffix = "_locked";
        }

        std::string path = "assets/images/" + folder + "/buttons/chapter" +
            std::to_string(i + 1) + stateSuffix + ".png";
        btnChapters[i]->reloadTexture(path);
    }
}

// MỞ KHÓA CHAPTER TIẾP THEO (gọi khi win)
void Challenge::unlockNextChapter() {
    if (highestUnlockedChapter < 4) {
        highestUnlockedChapter++;
        setTheme(currentTheme);   // Load lại layer 
    }
}

// XỬ LÝ SỰ KIỆN
GameState Challenge::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Đang mở popup điều kiện
    if (isPopupOpen) {
        if (btnClosePopup->isClicked(event, mousePos)) {
            isPopupOpen = false;
        }
        if (btnStart->isClicked(event, mousePos)) {
            return GameState::GAMEPLAY;
        }
        return GameState::CHALLENGE;
    }

    // Nút Close lớn → về Main Menu
    if (btnClose.isClicked(event, mousePos)) {
        return GameState::MAIN_MENU;
    }

    // Click vào 5 nút Chapter (chỉ cho phép màn đã mở)
    for (int i = 0; i < 5; ++i) {
        if (i <= highestUnlockedChapter) {
            if (btnChapters[i]->isClicked(event, mousePos)) {
                selectedChapter = i;
                isPopupOpen = true;

                // Nạp ảnh popup điều kiện của chapter
                std::string folder = getThemeFolder(currentTheme);
                std::string popupPath = "assets/images/" + folder +
                    "/popups/challenge/chapter_" + std::to_string(i + 1) + ".png";

                if (popupTexture.loadFromFile(popupPath)) {
                    popupTexture.setSmooth(true);
                    popupSprite.setTexture(popupTexture, true);

                    sf::FloatRect pBounds = popupSprite.getLocalBounds();
                    popupSprite.setOrigin(sf::Vector2f(
                        pBounds.position.x + pBounds.size.x / 2.f,
                        pBounds.position.y + pBounds.size.y / 2.f));
                    popupSprite.setPosition(sf::Vector2f(windowWidth / 2.f, windowHeight / 2.f));
                }
            }
        }
    }

    return GameState::CHALLENGE;
}

// UPDATE
void Challenge::update(sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (isPopupOpen) {
        btnStart->update(mousePos);
        btnClosePopup->update(mousePos);
    }
    else {
        btnClose.update(mousePos);
        for (int i = 0; i <= highestUnlockedChapter && i < 5; ++i) {
            btnChapters[i]->update(mousePos);
        }
    }
}

// RENDER
void Challenge::render(sf::RenderWindow& window) const {
    window.draw(bgSprite);
    btnClose.render(window);

    for (Button* btn : btnChapters) {
        btn->render(window);
    }

    if (isPopupOpen) {
        window.draw(dimOverlay);
        window.draw(popupSprite);
        btnStart->render(window);
        btnClosePopup->render(window);
    }
}