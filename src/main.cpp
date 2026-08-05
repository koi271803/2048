#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <vector>
#include <cctype> // Thư viện hỗ trợ hàm tolower (chuyển thành chữ thường)
#include "Grid.h"

using namespace std;

enum GameState { MENU, NEW_GAME, CHALLENGE, SETTINGS, HOW_TO_PLAY };

struct LevelConfig {
    int boardSize;
    int targetScore;
    int maxMoves;
};

void printMenu() {
    system("cls");
    cout << "======================================\n";
    cout << "               2 0 4 8      \n";
    cout << "======================================\n";
    cout << " [1] New Game (Select Size)\n";
    cout << " [2] Challenge (5 Epic Levels)\n";
    cout << " [3] Settings (Theme & Volume)\n";
    cout << " [4] How to Play\n";
    cout << "======================================\n";
    cout << "Choose an option (1-4): ";
}

void printGameScreen(Grid& game, int bestScore, int targetScore = 0, int maxMoves = 0) {
    system("cls");
    cout << " SCORE: " << game.getScore() << " \t\t BEST: " << bestScore << "\n";

    if (targetScore > 0) {
        cout << " TARGET: " << targetScore << " \t MOVES: " << game.getMovesCount() << " / " << maxMoves << "\n";
    }
    else {
        cout << " MOVES: " << game.getMovesCount() << "\n";
    }
    cout << "------------------------------------------------------\n";
    cout << " Power-ups: [1] Undo (" << game.getUndoCount() << ") | [2] Swap ("
        << game.getSwapCount() << ") | [3] Delete (" << game.getDeleteCount() << ")\n";
    cout << "------------------------------------------------------\n\n";

    for (int i = 0; i < game.getSize(); i++) {
        for (int j = 0; j < game.getSize(); j++) {
            int val = game.getTileValue(i, j);
            if (val == 0) cout << "[    ]\t";
            else cout << "[ " << val << " ]\t";
        }
        cout << "\n\n";
    }
    cout << "Move (WASD/Arrows) | Power-up (1/2/3) | Back to Menu (Q): ";
}

// Hàm dọn dẹp rác bộ đệm (Dùng khi người chơi lỡ nhập chữ thay vì số)
void clearInputBuffer() {
    cin.clear(); // Xóa cờ lỗi của cin
    cin.ignore(32767, '\n'); // Xóa toàn bộ ký tự bị kẹt cho đến khi gặp dấu Enter
}

void runGameplay(int size, int targetScore = 0, int maxMoves = 0) {
    Grid game(size);
    int bestScore = 0;
    bool playing = true;

    while (playing && !game.checkGameOver()) {
        if (targetScore > 0 && game.getMovesCount() >= maxMoves && game.getScore() < targetScore) {
            printGameScreen(game, bestScore, targetScore, maxMoves);
            cout << "\nOUT OF MOVES! YOU LOSE! Press any key...";
            (void)_getch();
            return;
        }

        if (game.getScore() > bestScore) bestScore = game.getScore();

        printGameScreen(game, bestScore, targetScore, maxMoves);

        // Nhận phím và chuyển ngay thành chữ thường để dễ xử lý (chống lỗi bật CapsLock)
        int action = _getch();

        // Bắt mã phím mũi tên (Mũi tên thường gửi 2 mã liên tiếp: 224 hoặc 0, sau đó là hướng)
        if (action == 224 || action == 0) {
            action = _getch();
            if (action == 72) game.shiftUp();
            else if (action == 80) game.shiftDown();
            else if (action == 75) game.shiftLeft();
            else if (action == 77) game.shiftRight();
        }
        else {
            action = tolower(action); // Chuẩn hóa về chữ thường: 'W' thành 'w'

            if (action == 'w') game.shiftUp();
            else if (action == 's') game.shiftDown();
            else if (action == 'a') game.shiftLeft();
            else if (action == 'd') game.shiftRight();

            else if (action == '1') {
                if (!game.useUndo()) { cout << "\nCannot Undo!"; (void)_getch(); }
            }
            else if (action == '2') {
                if (game.getSwapCount() > 0) {
                    int r1, c1, r2, c2;
                    cout << "\n[SWAP] Enter coords (r1 c1 r2 c2): ";
                    cin >> r1 >> c1 >> r2 >> c2;

                    if (cin.fail()) { // Bắt lỗi nhập chữ cái thay vì số
                        clearInputBuffer();
                        cout << "Input Error! Please enter numbers only.";
                        (void)_getch();
                    }
                    else if (!game.useSwap(r1, c1, r2, c2)) {
                        cout << "Invalid Coordinates!";
                        (void)_getch();
                    }
                }
            }
            else if (action == '3') {
                if (game.getDeleteCount() > 0) {
                    int r, c;
                    cout << "\n[DELETE] Enter coord (r c): ";
                    cin >> r >> c;

                    if (cin.fail()) {
                        clearInputBuffer();
                        cout << "Input Error! Please enter numbers only.";
                        (void)_getch();
                    }
                    else if (!game.useDelete(r, c)) {
                        cout << "Invalid Coordinates or Empty Tile!";
                        (void)_getch();
                    }
                }
            }
            else if (action == 'q') {
                playing = false;
            }
        }

        if (targetScore > 0 && game.getScore() >= targetScore && game.getMovesCount() <= maxMoves) {
            printGameScreen(game, bestScore, targetScore, maxMoves);
            cout << "\nLEVEL CLEARED! MEOW MEOW! Press any key...";
            (void)_getch();
            playing = false;
        }
    }

    if (game.checkGameOver()) {
        printGameScreen(game, bestScore, targetScore, maxMoves);
        cout << "\nGAME OVER! BOARD FULL! Press any key to return to Main Menu...";
        (void)_getch();
    }
}

int main() {
    GameState currentState = MENU;

    vector<LevelConfig> challengeLevels = {
        {4, 128,  50},
        {4, 256,  100},
        {4, 512,  150},
        {5, 1024, 250},
        {5, 2048, 400}
    };

    while (true) {
        if (currentState == MENU) {
            printMenu();
            char choice = _getch();
            switch (choice) {
            case '1': currentState = NEW_GAME; break;
            case '2': currentState = CHALLENGE; break;
            case '3': currentState = SETTINGS; break;
            case '4': currentState = HOW_TO_PLAY; break;
            }
        }
        else if (currentState == NEW_GAME) {
            system("cls");
            cout << "=== SELECT GRID SIZE ===\n";
            cout << "[1] 4x4   [2] 5x5   [3] 6x6\n";
            cout << "Choose (1-3): ";

            int size = 4;
            bool validChoice = false;
            while (!validChoice) {
                char sizeChoice = _getch();
                if (sizeChoice == '1') { size = 4; validChoice = true; }
                else if (sizeChoice == '2') { size = 5; validChoice = true; }
                else if (sizeChoice == '3') { size = 6; validChoice = true; }
            }

            runGameplay(size);
            currentState = MENU;
        }
        else if (currentState == CHALLENGE) {
            for (size_t i = 0; i < challengeLevels.size(); i++) {
                system("cls");
                cout << "=== CHALLENGE MODE ===\n";
                cout << "Starting Level " << i + 1 << "...\n";
                cout << "Target: " << challengeLevels[i].targetScore << " | Max Moves: " << challengeLevels[i].maxMoves << "\n";
                cout << "Press any key to start!";
                (void)_getch();

                runGameplay(challengeLevels[i].boardSize, challengeLevels[i].targetScore, challengeLevels[i].maxMoves);
            }
            currentState = MENU;
        }
        else if (currentState == SETTINGS) {
            system("cls");
            cout << "=== SETTINGS (THEME & VOLUME) ===\n";
            cout << "Themes: [1] Pastel Cat (Default)  [2] Dark Meow  [3] Ocean Cat\n";
            cout << "Press any key to return...\n";
            (void)_getch();
            currentState = MENU;
        }
        else if (currentState == HOW_TO_PLAY) {
            system("cls");
            cout << "=== HOW TO PLAY ===\n";
            cout << "Move: WASD or Arrows. Power-ups: 1, 2, 3.\n";
            cout << "Challenge Mode: Reach target score before you run out of moves!\n";
            cout << "Press any key to return...\n";
            (void)_getch();
            currentState = MENU;
        }
    }

    return 0;
}
