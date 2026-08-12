#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <AudioManager.h>
#include "GameState.h"
#include "Grid.h"
#include "Settings.h"
#include "Button.h"

class AudioManager;


// Popup PNG float lên (gộp ô 2048, 10k điểm...)
struct FloatingPopup {
    sf::Sprite sprite;
    float alpha;

    FloatingPopup(const sf::Texture& tex, sf::Vector2f pos)
        : sprite(tex), alpha(255.f)
    {
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f));
        sprite.setPosition(pos);
    }
};

// Text bay len khi +1 Power (+1 Undo / +1 Swap / +1 Delete)
struct FloatingText {
    sf::Text text;
    float alpha;
    float lifetime;

    FloatingText(const sf::Font& font, const std::string& str, sf::Vector2f pos)
        : text(font, str, 28)
        , alpha(255.f)
        , lifetime(2.2f)
    {
        text.setFillColor(sf::Color::White);
        text.setOutlineThickness(0.f);
        text.setOutlineColor(sf::Color::Transparent);
        text.setPosition(pos);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f));
    }
};

// GAMEPLAY: quan ly man choi (Free + Challenge)
class Gameplay {
private:
    // --- LOGIC ---
    int gridSize;
    Grid grid;
    static int bestScore;

    bool isChallengeMode;
    int currentMoves;
    int startMoves;                 // Luu so moves ban dau (de Restart Challenge)
    int targetTile;                 // 0 = free mode
    int currentMaxTile;

    // Luu so power ban dau cua chapter
    int initialUndoCount;
    int initialSwapCount;
    int initialDeleteCount;

    ThemeType currentTheme;
    bool isGameOverState;
    bool hasWonFlag;
    bool isWinState = false;

    // Audio
    AudioManager* audio;
	// Popup Win
    sf::Texture popupWinTex;
    sf::Sprite  popupWinSprite;
    Button* btnQuitWin = nullptr;
	// Popup Game Over
    sf::RectangleShape dimOverlay;
    sf::Texture popupGameOverTex;
    sf::Sprite  popupGameOverSprite;
    Button* btnReplay;
    Button* btnQuit;

    enum SubState {
        PLAYING,
        POPUP_HOME,
        POPUP_POWER_CONFIRM,
        WAIT_FIRST_TILE,
        WAIT_SECOND_TILE,
        WAIT_DELETE_TILE
    };
    SubState currentSubState;

    int activePowerType;            // -1, 0=Undo, 1=Swap, 2=Delete
    int selectedR1, selectedC1;
    int selectedR2, selectedC2;

    // Popup assets
    sf::Texture popupHomeTex, popupPowerConfirmTex;
    sf::Sprite  popupHomeSprite, popupPowerConfirmSprite;
    Button* btnYes;
    Button* btnNo;
    Button* btnClosePopup;

    // Milestone flags
    bool score10k, score15k, score25k, score40k, score60k;

    const sf::Font& font;

    // UI
    sf::Texture bgTexture, boardTexture;
    sf::Sprite  bgSprite, boardSprite;

    sf::Texture scoreBoxTexture, bestBoxTexture, movesBoxTexture;
    sf::Sprite  scoreBoxSprite, bestBoxSprite, movesBoxSprite;

    sf::Texture powerupsPanelTexture;
    sf::Sprite  powerupsPanelSprite;

    sf::Texture menuBoxTexture, homeIconTexture, settingsIconTexture;
    sf::Sprite  menuBoxSprite, homeIconSprite, settingsIconSprite;

    // Power-ups
    sf::Texture powerTextures[3];
    std::vector<sf::Sprite> powerSprites;
    enum PowerType { UNDO = 0, SWAP = 1, DELETE_TYPE = 2 };

    std::map<int, sf::Texture> tileTextures;
    std::map<int, sf::Texture> milestoneTextures;
    std::vector<FloatingPopup> floatingPopups;
    std::vector<FloatingText>  floatingTexts;

    // Texts
    sf::Text scoreText;
    sf::Text bestScoreText;
    sf::Text movesText;
    std::vector<sf::Text> countTexts;

    // Helpers
    void loadAllAssets(const std::string& folder = "green");
    void loadTileTextures(const std::string& folder = "green");
    void updateScoreTexts();
    void updateMovesText();
    void updatePowerCountsTexts();
    void checkMilestones();
    void checkGameOver();
    void checkWin();
    void spawnPowerText(const std::string& content);

public:
    // Constructor
    Gameplay(const sf::Font& fontRef,
        float windowWidth, float windowHeight,
        int size = 4,
        bool isChallenge = false,
        int startMoves = 0,
        int targetTile = 0,
        int undoCount = 3,
        int swapCount = 2,
        int deleteCount = 1,
        AudioManager* audioMgr = nullptr);

    ~Gameplay();

    void setTheme(ThemeType theme);
    void setAudio(AudioManager* audioMgr) { audio = audioMgr; }

    GameState handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void render(sf::RenderWindow& window) const;

    bool hasWon() const { return hasWonFlag; }
};
