#include <iostream>
#include <cstdlib>
#include "Core/ObstacleMode.h" // Chèn lớp logic bạn muốn test (Grid.h hoặc ObstacleMode.h)

using namespace std;

// Hàm tiện ích để in mảng 2 chiều ra console
void printBoard(Grid& game) {
    system("cls"); // Xóa màn hình console (Nếu dùng Linux/Mac thì đổi thành system("clear"))
    
    cout << "=== GAME 2048 - TEST LOGIC ===\n";
    cout << "Diem so: " << game.getScore() << "\n\n";
    
    for (int i = 0; i < game.getSize(); i++) {
        for (int j = 0; j < game.getSize(); j++) {
            int val = game.getTileValue(i, j);
            
            if (val == -1) {
                cout << "[ X ]\t"; // In chữ X cho Chướng ngại vật
            } else if (val == 0) {
                cout << "[   ]\t"; // Để trống nếu là ô số 0
            } else {
                cout << "[ " << val << " ]\t"; // In con số
            }
        }
        cout << "\n\n";
    }
}

int main() {
    // Khởi tạo chế độ chơi Chướng ngại vật (8x8)
    ObstacleGameMode game; 
    char input;

    // Vòng lặp game chính trên Console
    while (!game.checkGameOver()) {
        printBoard(game);
        
        cout << "Nhap huong di (W: Len, S: Xuong, A: Trai, D: Phai) hoac Q de thoat: ";
        cin >> input;

        // Xử lý phím bấm và gọi các hàm logic tương ứng
        if (input == 'w' || input == 'W') game.shiftUp();
        else if (input == 's' || input == 'S') game.shiftDown();
        else if (input == 'a' || input == 'A') game.shiftLeft();
        else if (input == 'd' || input == 'D') game.shiftRight();
        else if (input == 'q' || input == 'Q') break;
    }

    // Khi vòng lặp kết thúc (thua game)
    printBoard(game);
    cout << "GAME OVER! Khong con nuoc di nao. Tong diem: " << game.getScore() << endl;
    
    return 0;
}
