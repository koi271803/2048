#include "Gameplay.h"
#include "AudioManager.h"
#include <iostream>
#include <algorithm>
#include <cstdint>

int Gameplay::bestScore = 0;

// 1. CONSTRUCTOR
Gameplay::Gameplay(const sf::Font& fontRef,
    float windowWidth, float windowHeight,
    int size, bool isChallenge,
    int startMovesParam, int target,
    int undoCount, int swapCount, int deleteCount,
    AudioManager* audioMgr)
    : font(fontRef),
    gridSize(size),
    grid(size),
    isChallengeMode(isChallenge),
    currentMoves(isChallenge ? startMovesParam : 0),
    startMoves(startMovesParam),       
    targetTile(target),
    currentMaxTile(0),
    initialUndoCount(undoCount),
    initialSwapCount(swapCount),
    initialDeleteCount(deleteCount),
    currentTheme(THEME_GREEN),
    isGameOverState(false),
    hasWonFlag(false),
    audio(audioMgr),
    currentSubState(PLAYING),
    activePowerType(-1),
    selectedR1(-1), selectedC1(-1), selectedR2(-1), selectedC2(-1),

    bgTexture(), bgSprite(bgTexture),
    boardTexture(), boardSprite(boardTexture),
    scoreBoxTexture(), scoreBoxSprite(scoreBoxTexture),
    bestBoxTexture(), bestBoxSprite(bestBoxTexture),
    movesBoxTexture(), movesBoxSprite(movesBoxTexture),
    powerupsPanelTexture(), powerupsPanelSprite(powerupsPanelTexture),
    menuBoxTexture(), menuBoxSprite(menuBoxTexture),
    homeIconTexture(), homeIconSprite(homeIconTexture),
    settingsIconTexture(), settingsIconSprite(settingsIconTexture),
    popupGameOverTex(), popupGameOverSprite(popupGameOverTex),
    popupHomeTex(), popupHomeSprite(popupHomeTex),
    popupPowerConfirmTex(), popupPowerConfirmSprite(popupPowerConfirmTex),

    score10k(false), score15k(false), score25k(false), score40k(false), score60k(false),
    scoreText(fontRef, "0", 72),
    bestScoreText(fontRef, std::to_string(bestScore), 72),
    movesText(fontRef, "0", 72)
{
    // Gán số lượt Power theo Challenge / Free
    grid.setPowerCounts(undoCount, swapCount, deleteCount);

    // Nút Game Over
    btnReplay = new Button("assets/images/green/buttons/restart.png", fontRef, "",
        windowWidth / 2.0f + 140.f, windowHeight / 2.0f + 75.f);
    btnQuit = new Button("assets/images/green/buttons/quit_lose.png", fontRef, "",
        windowWidth / 2.0f - 140.f, windowHeight / 2.0f + 75.f);

    btnYes = new Button("assets/images/green/buttons/yes.png", fontRef, "",
        windowWidth / 2.0f + 120.f, windowHeight / 2.0f + 100.f);
    btnNo = new Button("assets/images/green/buttons/no.png", fontRef, "",
        windowWidth / 2.0f - 120.f, windowHeight / 2.0f + 100.f);

    btnClosePopup = new Button("assets/images/green/buttons/close.png", fontRef, "",
        windowWidth / 2.0f + 310.f, windowHeight / 2.0f - 175.f);

    setTheme(THEME_GREEN);

    // Background scale
    float scaleX = windowWidth / static_cast<float>(bgTexture.getSize().x);
    float scaleY = windowHeight / static_cast<float>(bgTexture.getSize().y);
    bgSprite.setScale(sf::Vector2f(scaleX, scaleY));

    // Board
    sf::FloatRect bBounds = boardSprite.getLocalBounds();
    boardSprite.setOrigin(sf::Vector2f(
        bBounds.position.x + bBounds.size.x / 2.f,
        bBounds.position.y + bBounds.size.y / 2.f));
    boardSprite.setPosition(sf::Vector2f(960.f, 540.f));

    // Boxes
    scoreBoxSprite.setPosition(sf::Vector2f(40.f, 450.f));
    bestBoxSprite.setPosition(sf::Vector2f(40.f, 675.f));
    movesBoxSprite.setPosition(sf::Vector2f(90.f, 220.f));

    // Texts
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(240.f, 550.f));

    bestScoreText.setFillColor(sf::Color::White);
    bestScoreText.setPosition(sf::Vector2f(240.f, 780.f));

    movesText.setFillColor(sf::Color::White);
    movesText.setPosition(sf::Vector2f(240.f, 315.f));

    // Power panel + Menu
    powerupsPanelSprite.setPosition(sf::Vector2f(1500.f, 500.f));
    menuBoxSprite.setPosition(sf::Vector2f(1600.f, 850.f));

    // Home / Settings icons
    sf::FloatRect hBounds = homeIconSprite.getLocalBounds();
    homeIconSprite.setOrigin(sf::Vector2f(
        hBounds.position.x + hBounds.size.x / 2.f,
        hBounds.position.y + hBounds.size.y / 2.f));
    homeIconSprite.setPosition(sf::Vector2f(1810.f, 950.f));

    sf::FloatRect sBounds = settingsIconSprite.getLocalBounds();
    settingsIconSprite.setOrigin(sf::Vector2f(
        sBounds.position.x + sBounds.size.x / 2.f,
        sBounds.position.y + sBounds.size.y / 2.f));
    settingsIconSprite.setPosition(sf::Vector2f(1690.f, 950.f));

    // 3 Power sprites + count texts
    float startXPower = 1590.f;
    float startYPower = 600.f;
    float spacingPower = 115.f;

    powerSprites.clear();
    countTexts.clear();

    for (int i = 0; i < 3; ++i) {
        powerSprites.emplace_back(powerTextures[i]);
        sf::FloatRect pBounds = powerSprites[i].getLocalBounds();
        powerSprites[i].setOrigin(sf::Vector2f(
            pBounds.position.x + pBounds.size.x / 2.f,
            pBounds.position.y + pBounds.size.y / 2.f));
        powerSprites[i].setPosition(sf::Vector2f(
            startXPower + static_cast<float>(i) * spacingPower, startYPower));

        countTexts.emplace_back(font, "0", 21);
        countTexts[i].setFillColor(sf::Color(28, 77, 46));
    }

    dimOverlay.setSize(sf::Vector2f(windowWidth, windowHeight));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 180));

    updatePowerCountsTexts();
    updateScoreTexts();
    updateMovesText();

    // Audio
    if (!audio)
        std::cout << "[Gameplay] CANH BAO: audio KHONG co SFX merge/lose/win!\n";
    else
        std::cout << "[Gameplay] Audio OK - SFX merge/lose/win se hoat dong.\n";
}

Gameplay::~Gameplay() {
    delete btnReplay;
    delete btnQuit;
    delete btnYes;
    delete btnNo;
    delete btnClosePopup;
}
// 2. SPAWN TEXT +1 POWER
void Gameplay::spawnPowerText(const std::string& content)
{
    // Vị trí góc trên bên phải (chỉnh tại đây)
    sf::Vector2f pos(1750.f, 180.f);

    // Nếu đã có text đang bay thì dịch xuống để không đè
    float offsetY = floatingTexts.size() * 45.f;
    pos.y += offsetY;

    floatingTexts.emplace_back(font, content, pos);

    auto& t = floatingTexts.back().text;
    t.setCharacterSize(32);        
    t.setOutlineThickness(0.f);     
    t.setOutlineColor(sf::Color::Transparent);
}

// 3. MILESTONES
void Gameplay::checkMilestones() {
    bool playedMilestone = false;

    int maxTile = grid.getMaxTile();
    if (maxTile > currentMaxTile) {
        // Popup khi gộp được ô số lớn  (2048, 4096, 8192...)
        if (maxTile >= 2048 && milestoneTextures.count(maxTile) > 0) {
            floatingPopups.emplace_back(milestoneTextures[maxTile], sf::Vector2f(960.f, 540.f));
            if (audio && !playedMilestone) {
                audio->playSound("milestone");
                playedMilestone = true;
            }
        }
        currentMaxTile = maxTile;
    }

    int score = grid.getScore();
	// Popup mốc điêm 10k, 15k, 25k, 40k, 60k
    auto tryScoreMilestone = [&](bool& flag, int threshold) {
        if (!flag && score >= threshold && milestoneTextures.count(threshold) > 0) {
            flag = true;
            floatingPopups.emplace_back(milestoneTextures[threshold], sf::Vector2f(960.f, 540.f));
            if (audio && !playedMilestone) {
                audio->playSound("milestone");
                playedMilestone = true;
            }
        }
        };
    tryScoreMilestone(score10k, 10000);
    tryScoreMilestone(score15k, 15000);
    tryScoreMilestone(score25k, 25000);
    tryScoreMilestone(score40k, 40000);
    tryScoreMilestone(score60k, 60000);
}

// 4. CHECK WIN (Challenge only)
void Gameplay::checkWin() {
    if (!isChallengeMode || hasWonFlag || isGameOverState) return;

    if (grid.getMaxTile() >= targetTile) {
        hasWonFlag = true;
        // Phat SFX win 
        if (audio) audio->playSound("win");
        // Hien tai main se bat hasWon() va unlock chapter
    }
}

// 5. CHECK GAME OVER
void Gameplay::checkGameOver() {
    if (isGameOverState || hasWonFlag) return;

    std::string folder = getThemeFolder(currentTheme);
    std::string popPath = "assets/images/" + folder + "/popups/gameplay/";
    std::string btnPath = "assets/images/" + folder + "/buttons/";

    bool lose = false;

    // Hết bước (Challenge)
    if (isChallengeMode && currentMoves <= 0) {
        lose = true;
    }
    // Kẹt bàn cờ
    else if (grid.isGameOver()) {
        lose = true;
    }

    if (lose) {
        isGameOverState = true;
        if (audio) audio->playSound("lose");
        (void)popupGameOverTex.loadFromFile(popPath + "lose.png");
        popupGameOverTex.setSmooth(true);
        popupGameOverSprite.setTexture(popupGameOverTex, true);

        sf::FloatRect bounds = popupGameOverSprite.getLocalBounds();
        popupGameOverSprite.setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f));
        popupGameOverSprite.setPosition(sf::Vector2f(960.f, 540.f));

        btnReplay->reloadTexture(btnPath + "restart.png");
        btnQuit->reloadTexture(btnPath + "quit_lose.png");
    }
}

// 6. HANDLE EVENT
GameState Gameplay::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // GAME OVER 
    if (isGameOverState) {
        if (btnReplay->isClicked(event, mousePos)) {
            if (audio) audio->playSound("click");
            // RESTART: load lại màn chơi từ đầu 
            grid.initBoard();

            // Challenge -> trả lại đúng số moves ban đầu của chapter
            // Free     -> reset bộ đếm moves về 0
            currentMoves = isChallengeMode ? startMoves : 0;

            // Trả lại số lượt Power ban đầu
            grid.setPowerCounts(initialUndoCount, initialSwapCount, initialDeleteCount);

            // Reset trạng thái
            isGameOverState = false;
            hasWonFlag = false;
            currentMaxTile = 0;
            currentSubState = PLAYING;
            activePowerType = -1;
            selectedR1 = selectedC1 = selectedR2 = selectedC2 = -1;

            // Reset milestone flags
            score10k = score15k = score25k = score40k = score60k = false;
            floatingPopups.clear();
            floatingTexts.clear();

            updateScoreTexts();
            updateMovesText();
            updatePowerCountsTexts();
        }
        if (btnQuit->isClicked(event, mousePos)) {
            if (isChallengeMode) return GameState::CHALLENGE;
            return GameState::MAIN_MENU;
        }
        return GameState::GAMEPLAY;
    }

    // POPUP HOME 
    if (currentSubState == POPUP_HOME) {
        if (btnYes->isClicked(event, mousePos)) {
            if (isChallengeMode) return GameState::CHALLENGE;
            return GameState::MAIN_MENU;
        }
        if (btnNo->isClicked(event, mousePos) || btnClosePopup->isClicked(event, mousePos)) {
            currentSubState = PLAYING;
        }
        return GameState::GAMEPLAY;
    }

    // POPUP POWER CONFIRM 
    if (currentSubState == POPUP_POWER_CONFIRM) {
        if (btnYes->isClicked(event, mousePos)) {
            if (activePowerType == PowerType::UNDO) {
                if (grid.useUndo()) {
                    updatePowerCountsTexts();
                    updateScoreTexts();
                    updateMovesText();
                }
                currentSubState = PLAYING;
            }
            else if (activePowerType == PowerType::SWAP) {
                currentSubState = WAIT_FIRST_TILE;
            }
            else if (activePowerType == PowerType::DELETE_TYPE) {
                currentSubState = WAIT_DELETE_TILE;
            }
        }
        if (btnNo->isClicked(event, mousePos) || btnClosePopup->isClicked(event, mousePos)) {
            currentSubState = PLAYING;
            activePowerType = -1;
        }
        return GameState::GAMEPLAY;
    }

    //  CHỌN Ô (SWAP / DELETE) 
    if (currentSubState == WAIT_FIRST_TILE ||
        currentSubState == WAIT_SECOND_TILE ||
        currentSubState == WAIT_DELETE_TILE)
    {
        if (event.is<sf::Event::MouseButtonPressed>()) {
            const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>();
            if (mouseBtn && mouseBtn->button == sf::Mouse::Button::Left) {
                sf::FloatRect boardBounds = boardSprite.getGlobalBounds();
                float padding = 15.f;
                float startX = boardBounds.position.x + padding;
                float startY = boardBounds.position.y + padding;
                float tileSize = (boardBounds.size.x - padding * 2.f) / static_cast<float>(gridSize);

                for (int r = 0; r < gridSize; ++r) {
                    for (int c = 0; c < gridSize; ++c) {
                        sf::FloatRect tileRect(
                            sf::Vector2f(startX + static_cast<float>(c) * tileSize,
                                startY + static_cast<float>(r) * tileSize),
                            sf::Vector2f(tileSize, tileSize));

                        if (tileRect.contains(mousePos)) {
                            if (currentSubState == WAIT_FIRST_TILE) {
                                if (grid.getTileValue(r, c) > 0) {
                                    selectedR1 = r; selectedC1 = c;
                                    currentSubState = WAIT_SECOND_TILE;
                                }
                            }
                            else if (currentSubState == WAIT_SECOND_TILE) {
                                if (grid.getTileValue(r, c) > 0) {
                                    selectedR2 = r; selectedC2 = c;
                                    if (grid.useSwap(selectedR1, selectedC1, selectedR2, selectedC2)) {
                                        updatePowerCountsTexts();
                                    }
                                    currentSubState = PLAYING;
                                    activePowerType = -1;
                                }
                            }
                            else if (currentSubState == WAIT_DELETE_TILE) {
                                if (grid.getTileValue(r, c) > 0) {
                                    if (grid.useDelete(r, c)) {
                                        updatePowerCountsTexts();
                                    }
                                    currentSubState = PLAYING;
                                    activePowerType = -1;
                                }
                            }
                        }
                    }
                }
            }
        }
        return GameState::GAMEPLAY;
    }

    // CLICK THƯỜNG 
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseBtn && mouseBtn->button == sf::Mouse::Button::Left) {

            // Home
            if (homeIconSprite.getGlobalBounds().contains(mousePos)) {
                currentSubState = POPUP_HOME;
                return GameState::GAMEPLAY;
            }

            // Settings
            if (settingsIconSprite.getGlobalBounds().contains(mousePos)) {
                return GameState::SETTINGS;
            }

            // Power-ups (chỉ khi còn lượt)
            if (powerSprites.size() >= 3) {
                std::string folder = getThemeFolder(currentTheme);
                std::string popPath = "assets/images/" + folder + "/popups/gameplay/";

                if (powerSprites[PowerType::UNDO].getGlobalBounds().contains(mousePos) &&
                    grid.getUndoCount() > 0)
                {
                    activePowerType = PowerType::UNDO;
                    currentSubState = POPUP_POWER_CONFIRM;
                    (void)popupPowerConfirmTex.loadFromFile(popPath + "undo.png");
                }
                else if (powerSprites[PowerType::SWAP].getGlobalBounds().contains(mousePos) &&
                    grid.getSwapCount() > 0)
                {
                    activePowerType = PowerType::SWAP;
                    currentSubState = POPUP_POWER_CONFIRM;
                    (void)popupPowerConfirmTex.loadFromFile(popPath + "swap.png");
                }
                else if (powerSprites[PowerType::DELETE_TYPE].getGlobalBounds().contains(mousePos) &&
                    grid.getDeleteCount() > 0)
                {
                    activePowerType = PowerType::DELETE_TYPE;
                    currentSubState = POPUP_POWER_CONFIRM;
                    (void)popupPowerConfirmTex.loadFromFile(popPath + "delete.png");
                }

                if (currentSubState == POPUP_POWER_CONFIRM) {
                    popupPowerConfirmTex.setSmooth(true);
                    popupPowerConfirmSprite.setTexture(popupPowerConfirmTex, true);
                    sf::FloatRect bounds = popupPowerConfirmSprite.getLocalBounds();
                    popupPowerConfirmSprite.setOrigin(sf::Vector2f(
                        bounds.position.x + bounds.size.x / 2.f,
                        bounds.position.y + bounds.size.y / 2.f));
                    popupPowerConfirmSprite.setPosition(sf::Vector2f(960.f, 540.f));
                }
            }
        }
    }

    // PHÍM DI CHUYỂN
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        bool moved = false;

        if (!isChallengeMode || currentMoves > 0) {
			// Luôn lưu điểm trước khi move để xác định có merge hay không
            const int scoreBefore = grid.getScore();
            const int maxBefore = grid.getMaxTile();

            if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::A)
                moved = grid.moveLeft();
            else if (key->code == sf::Keyboard::Key::Right || key->code == sf::Keyboard::Key::D)
                moved = grid.moveRight();
            else if (key->code == sf::Keyboard::Key::Up || key->code == sf::Keyboard::Key::W)
                moved = grid.moveUp();
            else if (key->code == sf::Keyboard::Key::Down || key->code == sf::Keyboard::Key::S)
                moved = grid.moveDown();

            if (moved) {
                updateScoreTexts();
                if (isChallengeMode) currentMoves--;
                else currentMoves++;
                updateMovesText();

                const auto& merged = grid.getLastMergedValues();
                bool playedSmall = false;
                bool playedLarge = false;

                if (audio) {
                    for (int val : merged) {
                        if (val >= 2048) {
                            if (!playedLarge) {
                                audio->playSound("merge_large");
                                playedLarge = true;
                            }
                        }
                        else if (val >= 4 && val <= 1024) {
                            if (!playedSmall) {
                                audio->playSound("merge_small");
                                playedSmall = true;
                            }
                        }
                    }

                    const int scoreAfter = grid.getScore();
                    if (!playedSmall && !playedLarge && scoreAfter > scoreBefore) {
                        if (grid.getMaxTile() >= 2048 && grid.getMaxTile() > maxBefore)
                            audio->playSound("merge_large");
                        else
                            audio->playSound("merge_small");
                    }
                }

               // THƯỞNG THÊM POWERS KHI GỌP ĐƯỢC 128 / 256 / 512
                for (int val : merged) {
                    if (val == 128) {
						// fFree mode hoặc Challenge có mở khóa powers -> thưởng
                        if (!isChallengeMode || initialUndoCount > 0) {
                            grid.addUndo(1);
                            spawnPowerText("+1 Undo");
                        }
                    }
                    else if (val == 256) {
                        if (!isChallengeMode || initialSwapCount > 0) {
                            grid.addSwap(1);
                            spawnPowerText("+1 Swap");
                        }
                    }
                    else if (val == 512) {
                        if (!isChallengeMode || initialDeleteCount > 0) {
                            grid.addDelete(1);
                            spawnPowerText("+1 Delete");
                        }
                    }
                }
                grid.clearLastMergedValues();
                updatePowerCountsTexts();

                checkMilestones();
                checkWin();
                checkGameOver();
            }
        }
    }

    return GameState::GAMEPLAY;
}

// UPDATE
void Gameplay::update(sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    grid.updateAnimations();

    if (isGameOverState) {
        btnReplay->update(mousePos);
        btnQuit->update(mousePos);
        return;
    }

    if (currentSubState == POPUP_HOME || currentSubState == POPUP_POWER_CONFIRM) {
        btnYes->update(mousePos);
        btnNo->update(mousePos);
        btnClosePopup->update(mousePos);
        return;
    }

    // Hover Home / Settings
    if (homeIconSprite.getGlobalBounds().contains(mousePos))
        homeIconSprite.setScale(sf::Vector2f(1.10f, 1.10f));
    else
        homeIconSprite.setScale(sf::Vector2f(1.0f, 1.0f));

    if (settingsIconSprite.getGlobalBounds().contains(mousePos))
        settingsIconSprite.setScale(sf::Vector2f(1.10f, 1.10f));
    else
        settingsIconSprite.setScale(sf::Vector2f(1.0f, 1.0f));

    // Hover + Gray-out Power (chỉ khóa khi là Challenge và count = 0)
    // HOVER + GRAY-OUT POWER 
    for (size_t i = 0; i < powerSprites.size(); ++i) {
        int count = 0;
        if (i == PowerType::UNDO)          count = grid.getUndoCount();
        else if (i == PowerType::SWAP)     count = grid.getSwapCount();
        else if (i == PowerType::DELETE_TYPE) count = grid.getDeleteCount();

        // Chỉ khóa (xám) khi Challenge VÀ count <= 0
        bool shouldLock = isChallengeMode && count <= 0;

        if (shouldLock) {
            powerSprites[i].setColor(sf::Color(100, 100, 100, 180));
            powerSprites[i].setScale(sf::Vector2f(1.0f, 1.0f));
            if (i < countTexts.size()) {
                countTexts[i].setScale(sf::Vector2f(1.0f, 1.0f));
                countTexts[i].setFillColor(sf::Color(80, 80, 80));
            }
        }
        else {
            // Free mode hoặc Challenge còn lượt dùng -> màu gốc
            powerSprites[i].setColor(sf::Color::White);
            if (i < countTexts.size())
                countTexts[i].setFillColor(sf::Color(28, 77, 46));

            // Hover phóng to
            if (powerSprites[i].getGlobalBounds().contains(mousePos)) {
                powerSprites[i].setScale(sf::Vector2f(1.15f, 1.15f));
                if (i < countTexts.size()) {
                    countTexts[i].setScale(sf::Vector2f(1.25f, 1.25f));
                    float origX = (i == 0) ? 1560.f : (i == 1) ? 1672.f : 1788.f;
                    countTexts[i].setPosition(sf::Vector2f(origX - 4.f, 569.f - 4.f));
                }
            }
            else {
                powerSprites[i].setScale(sf::Vector2f(1.0f, 1.0f));
                if (i < countTexts.size()) {
                    countTexts[i].setScale(sf::Vector2f(1.0f, 1.0f));
                    float origX = (i == 0) ? 1560.f : (i == 1) ? 1672.f : 1788.f;
                    countTexts[i].setPosition(sf::Vector2f(origX, 569.f));
                }
            }
        }
    }

    // Floating Popup 
    for (int i = static_cast<int>(floatingPopups.size()) - 1; i >= 0; --i) {
        floatingPopups[i].sprite.move(sf::Vector2f(0.f, -1.5f));
        floatingPopups[i].alpha -= 1.6f;          

        if (floatingPopups[i].alpha <= 0.f) {
            floatingPopups.erase(floatingPopups.begin() + i);
        }
        else {
            std::uint8_t a = static_cast<std::uint8_t>(
                std::clamp(floatingPopups[i].alpha, 0.0f, 255.0f));
            floatingPopups[i].sprite.setColor(sf::Color(255, 255, 255, a));
        }
    }

    // Floating Text (+1 Power)
    for (int i = static_cast<int>(floatingTexts.size()) - 1; i >= 0; --i) {
        floatingTexts[i].lifetime -= 1.f / 60.f;
        floatingTexts[i].text.move(sf::Vector2f(0.f, -1.2f));
        floatingTexts[i].alpha -= 1.8f;

        if (floatingTexts[i].lifetime <= 0.f || floatingTexts[i].alpha <= 0.f) {
            floatingTexts.erase(floatingTexts.begin() + i);
        }
        else {
            std::uint8_t a = static_cast<std::uint8_t>(
                std::clamp(floatingTexts[i].alpha, 0.0f, 255.0f));
            floatingTexts[i].text.setFillColor(sf::Color(255, 255, 255, a));
        }
    }
}
// RENDER

void Gameplay::render(sf::RenderWindow& window) const {
    window.draw(bgSprite);
    window.draw(boardSprite);

    sf::FloatRect boardBounds = boardSprite.getGlobalBounds();
    float padding = 15.f;
    float startX = boardBounds.position.x + padding;
    float startY = boardBounds.position.y + padding;
    float tileSize = (boardBounds.size.x - padding * 2.f) / static_cast<float>(gridSize);

    // Vẽ tiles
    for (int r = 0; r < gridSize; ++r) {
        for (int c = 0; c < gridSize; ++c) {
            int value = grid.getTileValue(r, c);
            if (value > 0 && tileTextures.count(value) > 0) {
                sf::Sprite tSprite(tileTextures.at(value));
                float popScale = grid.getTileScale(r, c);
                float baseScale = (tileSize - 5.f) /
                    static_cast<float>(tSprite.getTexture().getSize().x);

                // Hiệu ứng chọn khi Swap / Delete
                if (currentSubState == WAIT_FIRST_TILE ||
                    currentSubState == WAIT_SECOND_TILE ||
                    currentSubState == WAIT_DELETE_TILE)
                {
                    bool isSelected = (r == selectedR1 && c == selectedC1) ||
                        (r == selectedR2 && c == selectedC2);

                    if (isSelected) {
                        baseScale *= 1.15f;
                        tSprite.setColor(sf::Color::White);
                    }
                    else {
                        baseScale *= 0.85f;
                        tSprite.setColor(sf::Color(150, 150, 150));
                    }
                }
                else {
                    tSprite.setColor(sf::Color::White);
                }

                tSprite.setScale(sf::Vector2f(baseScale * popScale, baseScale * popScale));

                sf::FloatRect bounds = tSprite.getLocalBounds();
                tSprite.setOrigin(sf::Vector2f(
                    bounds.position.x + bounds.size.x / 2.f,
                    bounds.position.y + bounds.size.y / 2.f));

                float actualSize = tileSize - 5.f;
                tSprite.setPosition(sf::Vector2f(
                    startX + static_cast<float>(c) * tileSize + 2.f + actualSize / 2.f,
                    startY + static_cast<float>(r) * tileSize + 2.f + actualSize / 2.f));

                window.draw(tSprite);
            }
        }
    }

    // UI boxes
    window.draw(scoreBoxSprite);
    window.draw(bestBoxSprite);
    window.draw(movesBoxSprite);
    window.draw(powerupsPanelSprite);
    window.draw(menuBoxSprite);

    window.draw(homeIconSprite);
    window.draw(settingsIconSprite);

    for (const auto& p : powerSprites) window.draw(p);
    window.draw(scoreText);
    window.draw(bestScoreText);
    window.draw(movesText);
    for (const auto& t : countTexts) window.draw(t);

    // Floating popup milestone
    for (const auto& fp : floatingPopups) window.draw(fp.sprite);

    // Floating text +1 Power
    for (const auto& ft : floatingTexts) window.draw(ft.text);

    // Popups
    if (currentSubState == POPUP_HOME) {
        window.draw(dimOverlay);
        window.draw(popupHomeSprite);
        btnYes->render(window);
        btnNo->render(window);
        btnClosePopup->render(window);
    }
    else if (currentSubState == POPUP_POWER_CONFIRM) {
        window.draw(dimOverlay);
        window.draw(popupPowerConfirmSprite);
        btnYes->render(window);
        btnNo->render(window);
        btnClosePopup->render(window);
    }

    // Game Over: 
    if (isGameOverState) {
        window.draw(dimOverlay);
        window.draw(popupGameOverSprite);
        btnReplay->render(window);
        btnQuit->render(window);
    }
}

// LOAD ASSETS

void Gameplay::setTheme(ThemeType theme) {
    currentTheme = theme;
    std::string folder = getThemeFolder(theme);
    loadAllAssets(folder);
    loadTileTextures(folder);
}

void Gameplay::loadAllAssets(const std::string& folder) {
    // Background
    if (bgTexture.loadFromFile("assets/images/" + folder + "/backgrounds/gameplay/gameplay_bg.png")) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture, true);
        float scaleX = 1920.f / static_cast<float>(bgTexture.getSize().x);
        float scaleY = 1080.f / static_cast<float>(bgTexture.getSize().y);
        bgSprite.setScale(sf::Vector2f(scaleX, scaleY));
        bgSprite.setPosition(sf::Vector2f(0.f, 0.f));
    }

    // Board
    std::string boardPath = "assets/images/" + folder + "/backgrounds/gameplay/board_" +
        std::to_string(gridSize) + "x" + std::to_string(gridSize) + ".png";
    if (boardTexture.loadFromFile(boardPath)) {
        boardTexture.setSmooth(true);
        boardSprite.setTexture(boardTexture, true);
        sf::FloatRect bBounds = boardSprite.getLocalBounds();
        boardSprite.setOrigin(sf::Vector2f(
            bBounds.position.x + bBounds.size.x / 2.f,
            bBounds.position.y + bBounds.size.y / 2.f));
        boardSprite.setPosition(sf::Vector2f(960.f, 540.f));
    }

    // Score / Best / Moves
    if (scoreBoxTexture.loadFromFile("assets/images/" + folder + "/backgrounds/gameplay/score.png")) {
        scoreBoxTexture.setSmooth(true);
        scoreBoxSprite.setTexture(scoreBoxTexture, true);
    }
    if (bestBoxTexture.loadFromFile("assets/images/" + folder + "/backgrounds/gameplay/best_score.png")) {
        bestBoxTexture.setSmooth(true);
        bestBoxSprite.setTexture(bestBoxTexture, true);
    }
    if (movesBoxTexture.loadFromFile("assets/images/" + folder + "/backgrounds/gameplay/moves.png")) {
        movesBoxTexture.setSmooth(true);
        movesBoxSprite.setTexture(movesBoxTexture, true);
    }

    // Power panel + Menu
    if (powerupsPanelTexture.loadFromFile("assets/images/" + folder + "/backgrounds/gameplay/powers.png")) {
        powerupsPanelTexture.setSmooth(true);
        powerupsPanelSprite.setTexture(powerupsPanelTexture, true);
    }
    if (menuBoxTexture.loadFromFile("assets/images/" + folder + "/backgrounds/gameplay/menu.png")) {
        menuBoxTexture.setSmooth(true);
        menuBoxSprite.setTexture(menuBoxTexture, true);
    }

    // Icons
    if (homeIconTexture.loadFromFile("assets/images/" + folder + "/buttons/home.png")) {
        homeIconTexture.setSmooth(true);
        homeIconSprite.setTexture(homeIconTexture, true);
    }
    if (settingsIconTexture.loadFromFile("assets/images/" + folder + "/buttons/settings_but.png")) {
        settingsIconTexture.setSmooth(true);
        settingsIconSprite.setTexture(settingsIconTexture, true);
    }

    // Power textures
    const char* powerNames[3] = { "undo", "swap", "delete" };
    for (int i = 0; i < 3; ++i) {
        std::string path = "assets/images/" + folder + "/buttons/" + powerNames[i] + ".png";
        if (powerTextures[i].loadFromFile(path)) {
            powerTextures[i].setSmooth(true);
            if (i < (int)powerSprites.size())
                powerSprites[i].setTexture(powerTextures[i], true);
        }
    }

    // Home popup
    if (popupHomeTex.loadFromFile("assets/images/" + folder + "/popups/gameplay/return_home.png")) {
        popupHomeTex.setSmooth(true);
        popupHomeSprite.setTexture(popupHomeTex, true);
        sf::FloatRect bounds = popupHomeSprite.getLocalBounds();
        popupHomeSprite.setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f));
        popupHomeSprite.setPosition(sf::Vector2f(960.f, 540.f));
    }

    // Reload buttons
    std::string btnPath = "assets/images/" + folder + "/buttons/";
    btnReplay->reloadTexture(btnPath + "restart.png");
    btnQuit->reloadTexture(btnPath + "quit_lose.png");
    btnYes->reloadTexture(btnPath + "yes.png");
    btnNo->reloadTexture(btnPath + "no.png");
    btnClosePopup->reloadTexture(btnPath + "close.png");
}

void Gameplay::loadTileTextures(const std::string& folder) {
    tileTextures.clear();
    milestoneTextures.clear();

    // Tiles 2 -> 65536
    int values[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };
    for (int v : values) {
        std::string path = "assets/images/" + folder + "/tiles/" + std::to_string(v) + ".png";
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            tex.setSmooth(true);
            tileTextures[v] = std::move(tex);
        }
    }

    // Milestone popups 
    int milestones[] = { 2048, 4096, 8192, 16384, 32768, 65536, 10000, 15000, 25000, 40000, 60000, 100000 };
    for (int m : milestones) {
        std::string path = "assets/images/" + folder + "/popups/gameplay/" + std::to_string(m) + ".png";
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            tex.setSmooth(true);
            milestoneTextures[m] = std::move(tex);
        }
    }
}


//  UPDATE TEXTS
void Gameplay::updateScoreTexts() {
    int score = grid.getScore();
    scoreText.setString(std::to_string(score));
    if (score > bestScore) {
        bestScore = score;
    }
    bestScoreText.setString(std::to_string(bestScore));

    // Căn giữa cả 2 text
    sf::FloatRect b1 = scoreText.getLocalBounds();
    scoreText.setOrigin(sf::Vector2f(b1.position.x + b1.size.x / 2.f,
        b1.position.y + b1.size.y / 2.f));
    sf::FloatRect b2 = bestScoreText.getLocalBounds();
    bestScoreText.setOrigin(sf::Vector2f(b2.position.x + b2.size.x / 2.f,
        b2.position.y + b2.size.y / 2.f));
}

void Gameplay::updateMovesText() {
    movesText.setString(std::to_string(currentMoves));
    sf::FloatRect bounds = movesText.getLocalBounds();
    movesText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f));
}

void Gameplay::updatePowerCountsTexts() {
    if (countTexts.size() < 3) return;

    countTexts[0].setString(std::to_string(grid.getUndoCount()));
    countTexts[1].setString(std::to_string(grid.getSwapCount()));
    countTexts[2].setString(std::to_string(grid.getDeleteCount()));

    // Vị trí chữ số trên icon
    float positionsX[3] = { 1560.f, 1672.f, 1788.f };
    for (int i = 0; i < 3; ++i) {
        countTexts[i].setPosition(sf::Vector2f(positionsX[i], 569.f));
        sf::FloatRect bounds = countTexts[i].getLocalBounds();
        countTexts[i].setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f));
    }
}
