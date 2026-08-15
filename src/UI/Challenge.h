#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Button.h"
#include "GameState.h"
#include "Settings.h"

// CẤU TRÚC ĐIỀU KIỆN TỪNG CHAPTER (khớp với ảnh)
struct ChapterData {
    int targetTile;     // Ô số cần đạt (256, 1024, 2048, 4096, 8192)
    int moveLimit;      // Số bước giới hạn
    int gridSize;       // Kích thước bàn cờ (4, 5, 6)
    int undoCount;      // Số lượt Undo ban đầu
    int swapCount;      // Số lượt Swap ban đầu
    int deleteCount;    // Số lượt Delete ban đầu
};

// LỚP CHALLENGE: BẢN ĐỒ 5 CHAPTER + 3 LAYER + POPUP ĐIỀU KIỆN
class Challenge {
private:
    ThemeType currentTheme;
    const sf::Font& font;
    float windowWidth, windowHeight;

    // Tiến trình người chơi
    int highestUnlockedChapter;          // 0 = Chapter 1 đang mở

    // Giao diện chính
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    Button btnClose;                     // Nút thoát về Main Menu
    std::vector<Button*> btnChapters;    // 5 nút Chapter
    std::vector<ChapterData> chapters;   // Dữ liệu 5 màn

    // Popup điều kiện
    bool isPopupOpen;
    int selectedChapter;

    sf::RectangleShape dimOverlay;
    sf::Texture popupTexture;
    sf::Sprite popupSprite;

    Button* btnStart;
    Button* btnClosePopup;

    void initChapterData();              // Khởi tạo điều kiện 5 màn

public:
    Challenge(const sf::Font& fontRef, float width, float height);
    ~Challenge();

    void setTheme(ThemeType theme);
    void unlockNextChapter();            // Gọi khi thắng để mở khóa màn tiếp
    void setHighestUnlocked(int value);
    int  getHighestUnlocked() const { return highestUnlockedChapter; }
    GameState handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window) const;
    void closePopup() { isPopupOpen = false; }
    int getSelectedChapter() const { return selectedChapter; }
    ChapterData getSelectedChapterData() const { return chapters[selectedChapter]; }
};
