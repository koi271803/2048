#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

#include "ThemeLoading.h"
#include "GameState.h"
#include "LoadingScreen.h"
#include "MainMenu.h"
#include "ModeSelect.h"
#include "Challenge.h"
#include "HowToPlay.h"
#include "Gameplay.h"
#include "Settings.h"
#include "AudioManager.h"
#include "SaveManager.h"

int main()
{
    // CẤU HÌNH CỬA SỔ
    const unsigned int WIDTH = 1920;
    const unsigned int HEIGHT = 1080;

    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "2048", sf::Style::Close);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // FONT
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Super Kidpop.ttf"))
    {
        std::cerr << "[Loi] Khong the mo font!\n";
        return -1;
    }

    // AUDIO
    AudioManager audio;
    audio.playLoadingMusic(true);
    audio.setMusicVolume(50.f);
    audio.setSfxVolume(50.f);

    // LOAD TIẾN TRÌNH ĐÃ LƯU
    SaveData saveData;
    SaveManager::load(saveData);

    ThemeType currentTheme = saveData.theme;
    ThemeType pendingTheme = saveData.theme;
    bool returnToSettingsAfterLoading = false;

    // Áp dụng volume đã lưu
    audio.setMusicVolume(saveData.musicVolume);
    audio.setSfxVolume(saveData.sfxVolume);

    // Áp dụng best score đã lưu
    Gameplay::setBestScore(saveData.bestScore);

    // CÁC MÀN HÌNH
    LoadingScreen loading(font, (float)WIDTH, (float)HEIGHT,
        "assets/images/green/backgrounds/loading/bg.png");

    MainMenu      mainMenu(font);
    ModeSelect    modeSelect(font);
    Challenge     challenge(font, (float)WIDTH, (float)HEIGHT);
    HowToPlay     howToPlay(font, (float)WIDTH, (float)HEIGHT);
    SettingsPopup settings(font, (float)WIDTH, (float)HEIGHT);
    ThemeLoading  themeLoading(font, (float)WIDTH, (float)HEIGHT);

    // Áp dụng chapter đã mở khóa
    challenge.setHighestUnlocked(saveData.highestUnlockedChapter);

    sf::Clock frameClock;

    // Áp dụng theme đã lưu
    mainMenu.setTheme(currentTheme);
    modeSelect.setTheme(currentTheme);
    challenge.setTheme(currentTheme);
    howToPlay.setTheme(currentTheme);
    settings.setTheme(currentTheme);

    std::unique_ptr<Gameplay> gameplay = nullptr;
    GameState currentState = GameState::LOADING;

    // HÀM LƯU TIẾN TRÌNH (gọi khi cần)

    auto saveProgress = [&]()
        {
            SaveData data;
            data.bestScore = Gameplay::getBestScore();
            data.highestUnlockedChapter = challenge.getHighestUnlocked();
            data.theme = currentTheme;
            data.musicVolume = audio.getMusicVolume();
            data.sfxVolume = audio.getSfxVolume();
            SaveManager::save(data);
        };

    // GAME LOOP
    while (window.isOpen())
    {
        float dt = frameClock.restart().asSeconds();

    
        // 1. ĐANG THEME-LOADING → chỉ update + render loading

        if (themeLoading.isActive())
        {
            themeLoading.update(dt);

            window.clear();
            themeLoading.render(window);
            window.display();
            continue;
        }

        // 2. VỪA XONG LOADING → APPLY THEME + MỞ LẠI SETTINGS
        if (themeLoading.isFinished())
        {
            currentTheme = pendingTheme;

            mainMenu.setTheme(currentTheme);
            modeSelect.setTheme(currentTheme);
            challenge.setTheme(currentTheme);
            howToPlay.setTheme(currentTheme);
            settings.setTheme(currentTheme);

            if (gameplay)
                gameplay->setTheme(currentTheme);

            if (returnToSettingsAfterLoading)
            {
                settings.open();
                returnToSettingsAfterLoading = false;
            }

            themeLoading.clearFinished();
            saveProgress();   // luu theme moi
        }


        // EVENT
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                saveProgress();   // luu truoc khi thoat
                window.close();
            }

            // SETTINGS
            if (settings.isOpen())
            {
                settings.handleEvent(*event, window);

                audio.setMusicVolume(settings.getMusicVolume());
                audio.setSfxVolume(settings.getSfxVolume());

                if (settings.hasThemeChangeRequest())
                {
                    ThemeType newTheme = settings.getRequestedTheme();

                    returnToSettingsAfterLoading = true;
                    settings.close();
                    settings.clearThemeChangeRequest();

                    pendingTheme = newTheme;
                    themeLoading.start(newTheme, 1.2f);
                }
                continue;
            }

            // CÁC MÀN HÌNH KHÁC 
            switch (currentState)
            {
            case GameState::LOADING:
            {
                GameState next = loading.handleEvent(*event);
                if (next == GameState::MAIN_MENU)
                {
                    audio.playSound("click");
                    audio.playMainMusic(true);
                }
                currentState = next;
                break;
            }

            case GameState::MAIN_MENU:
            {
                MenuOption opt = mainMenu.handleEvent(*event, window);

                if (opt == MenuOption::OPT_NEW_GAME)
                {
                    audio.playSound("click");
                    currentState = GameState::MODE_SELECT;
                }
                else if (opt == MenuOption::OPT_CHALLENGE)
                {
                    audio.playSound("click");
                    currentState = GameState::CHALLENGE;
                }
                else if (opt == MenuOption::OPT_HOW_TO_PLAY)
                {
                    audio.playSound("click");
                    howToPlay.reset();
                    howToPlay.setTheme(currentTheme);
                    currentState = GameState::HOW_TO_PLAY;
                }
                else if (opt == MenuOption::OPT_SETTINGS)
                {
                    audio.playSound("click");
                    settings.setTheme(currentTheme);
                    settings.open();
                }
                break;
            }

            case GameState::MODE_SELECT:
            {
                ModeOption m = modeSelect.handleEvent(*event, window);

                if (m == ModeOption::MODE_4x4 || m == ModeOption::MODE_5x5 || m == ModeOption::MODE_6x6)
                {
                    audio.playSound("click");
                    int size = (m == ModeOption::MODE_4x4) ? 4 :
                        (m == ModeOption::MODE_5x5) ? 5 : 6;

                    gameplay = std::make_unique<Gameplay>(
                        font, (float)WIDTH, (float)HEIGHT,
                        size, false, 0, 0,
                        3, 2, 1,
                        &audio
                    );
                    gameplay->setTheme(currentTheme);
                    currentState = GameState::GAMEPLAY;
                }
                else if (m == ModeOption::BACK)
                {
                    audio.playSound("click");
                    currentState = GameState::MAIN_MENU;
                }
                break;
            }

            case GameState::CHALLENGE:
            {
                GameState next = challenge.handleEvent(*event, window);

                if (next == GameState::GAMEPLAY)
                {
                    audio.playSound("click");
                    ChapterData data = challenge.getSelectedChapterData();

                    gameplay = std::make_unique<Gameplay>(
                        font, (float)WIDTH, (float)HEIGHT,
                        data.gridSize, true,
                        data.moveLimit,
                        data.targetTile,
                        data.undoCount, data.swapCount, data.deleteCount,
                        &audio
                    );
                    gameplay->setTheme(currentTheme);
                    currentState = GameState::GAMEPLAY;
                }
                else if (next == GameState::MAIN_MENU)
                {
                    audio.playSound("click");
                    currentState = next;
                }
                else
                {
                    currentState = next;
                }
                break;
            }

            case GameState::HOW_TO_PLAY:
            {
                GameState next = howToPlay.handleEvent(*event, window);
                if (next == GameState::MAIN_MENU)
                    audio.playSound("click");
                currentState = next;
                break;
            }

            case GameState::GAMEPLAY:
                if (gameplay)
                {
                    GameState next = gameplay->handleEvent(*event, window);

                    // Nguoi choi bam Quit tren popup You Win
                    if (next == GameState::CHALLENGE)
                    {
                        if (gameplay->hasWon())
                        {
                            challenge.unlockNextChapter();
                            saveProgress();   // luu chapter moi mo

                        }
                        challenge.closePopup();
                        audio.playSound("click");
                        currentState = GameState::CHALLENGE;
                        gameplay.reset();
                    }
                    else if (next == GameState::SETTINGS)
                    {
                        settings.setTheme(currentTheme);
                        settings.open();
                    }
                    else if (next != GameState::GAMEPLAY)
                    {
                        // Thoat ve Main Menu / Challenge → luu best score
                        saveProgress();

                        if (next == GameState::MAIN_MENU)
                            audio.playSound("click");
                        currentState = next;
                        gameplay.reset();
                    }
                }
                break;

            default:
                break;
            }
        }

        // UPDATE
        switch (currentState)
        {
        case GameState::LOADING:      loading.update();                 break;
        case GameState::MAIN_MENU:     mainMenu.update(window);          break;
        case GameState::MODE_SELECT:   modeSelect.update(window);        break;
        case GameState::CHALLENGE:     challenge.update(window);         break;
        case GameState::HOW_TO_PLAY:   howToPlay.update(window);         break;
        case GameState::GAMEPLAY:
            if (gameplay) gameplay->update(window);
            break;
        default: break;
        }

        if (settings.isOpen())
            settings.update(window);

        // RENDER
        window.clear();

        switch (currentState)
        {
        case GameState::LOADING:
            loading.render(window);
            break;

        case GameState::MAIN_MENU:
            mainMenu.render(window);
            break;

        case GameState::MODE_SELECT:
            mainMenu.render(window);
            modeSelect.render(window);
            break;

        case GameState::CHALLENGE:
            challenge.render(window);
            break;

        case GameState::HOW_TO_PLAY:
            howToPlay.render(window);
            break;

        case GameState::GAMEPLAY:
            if (gameplay) gameplay->render(window);
            break;

        default:
            break;
        }

        // Settings luôn vẽ trên cùng
        if (settings.isOpen())
            settings.render(window);

        window.display();
    }

    // Luu lan cuoi truoc khi thoat
    saveProgress();
    return 0;
}