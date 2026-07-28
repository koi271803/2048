#include <iostream>
#include <cstdlib>
#include "Grid.h"
#include <conio.h>

using namespace std;

// Hàm in Main Menu
void printMainMenu() {
    system("cls");
    cout << "================================\n";
    cout << "           2048 GAME            \n";
    cout << "================================\n";
    cout << " 1. New Game\n";
    cout << " 2. Continue\n";
    cout << " 3. How to play\n";
    cout << " 4. Settings\n";
    cout << " 5. Quit\n";
    cout << "================================\n";
    cout << "Choose an option (1-5): ";
}

// Hàm in màn hình chơi chính
void printGameScreen(Grid& game, int bestScore, int undoAmt, int swapAmt, int deleteAmt) {
    system("cls");

    // Header chứa điểm số
    cout << " SCORE: " << game.getScore() << " \t\t BEST: " << bestScore << "\n";
    cout << "--------------------------------\n";

    // Khu vực Power-ups
    cout << " Power-ups:\n";
    cout << " [1] Undo: " << undoAmt << "  |  [2] Swap: " << swapAmt << "  |  [3] Delete: " << deleteAmt << "\n";
    cout << "--------------------------------\n\n";

    // In bàn cờ 4x4
    for (int i = 0; i < game.getSize(); i++) {
        for (int j = 0; j < game.getSize(); j++) {
            int val = game.getTileValue(i, j);
            if (val == 0) {
                cout << "[    ]\t";
            }
            else {
                cout << "[ " << val << " ]\t";
            }
        }
        cout << "\n\n";
    }

    cout << "Move (W/A/S/D) | Use Power-up (1/2/3) | Quit to Menu (Q): ";
}

int main() {
    int bestScore = 0; // Tạm thời lưu ở biến, sau này có thể đọc/ghi từ file txt
    bool isRunning = true;

    while (isRunning) {
        printMainMenu();
        char menuChoice;
        cin >> menuChoice;

        if (menuChoice == '1') {
            // NEW GAME: Khởi tạo bàn cờ 4x4
            Grid game(4);

            // Khởi tạo số lượng Power-ups mặc định
            int undoCount = 2;
            int swapCount = 0;
            int deleteCount = 0;

            bool playing = true;
            char action;

            while (playing && !game.checkGameOver()) {
                printGameScreen(game, bestScore, undoCount, swapCount, deleteCount);

                // Dùng _getch() thay cho cin, game sẽ nhận phím ngay lập tức không cần Enter
                int action = _getch();

                // Cập nhật Best Score
                if (game.getScore() > bestScore) {
                    bestScore = game.getScore();
                }

                // Nếu người dùng bấm phím mũi tên (Mã mở rộng bắt đầu bằng 224)
                if (action == 224) {
                    action = _getch(); // Bắt buộc đọc thêm byte thứ 2 để biết là mũi tên nào

                    if (action == 72) game.shiftUp();        // Mũi tên Lên
                    else if (action == 80) game.shiftDown(); // Mũi tên Xuống
                    else if (action == 75) game.shiftLeft(); // Mũi tên Trái
                    else if (action == 77) game.shiftRight();// Mũi tên Phải
                }
                // Nếu người dùng bấm WASD hoặc phím chức năng
                else {
                    if (action == 'w' || action == 'W') game.shiftUp();
                    else if (action == 's' || action == 'S') game.shiftDown();
                    else if (action == 'a' || action == 'A') game.shiftLeft();
                    else if (action == 'd' || action == 'D') game.shiftRight();

                    // Phím tắt dùng Power-ups
                    else if (action == '1') {
                        // TODO: Xử lý Undo
                    }
                    else if (action == '2') {
                        // TODO: Xử lý Swap
                    }
                    else if (action == '3') {
                        // TODO: Xử lý Delete
                    }

                    // Thoát về menu
                    else if (action == 'q' || action == 'Q') playing = false;
                }
                // TODO: Xử lý logic dùng Power-ups ở đây (Nhấn 1, 2, 3)
            }

            if (game.checkGameOver()) {
                printGameScreen(game, bestScore, undoCount, swapCount, deleteCount);
                cout << "\nGAME OVER! Press any key to return to Main Menu...";
                cin.ignore();
                cin.get();
            }

        }
        else if (menuChoice == '5') {
            isRunning = false; // Thoát game
        }
        else {
            cout << "Feature under development! Press Enter to continue...\n";
            cin.ignore();
            cin.get();
        }
    }

    cout << "Thanks for playing!\n";
    return 0;
}
