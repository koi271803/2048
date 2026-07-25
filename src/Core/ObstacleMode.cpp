#include "ObstacleMode.h"
#include <cstdlib>
#include <vector>

// Hàm khởi tạo: Gọi lớp cha Grid với tham số truyền cứng là 8 (lưới 8x8)
ObstacleGameMode::ObstacleGameMode() : Grid(8) {
    // Khởi tạo thêm 3 cục đá ngay từ đầu game để tăng độ khó
    spawnRandomTile();
    spawnRandomTile();
    spawnRandomTile();
}

// Ghi đè hàm sinh số ngẫu nhiên
void ObstacleGameMode::spawnRandomTile() {
    std::vector<std::pair<int, int>> emptyCells;

    // Tìm các ô trống (giá trị = 0)
    for (int i = 0; i < getSize(); i++) {
        for (int j = 0; j < getSize(); j++) {
            if (getTileValue(i, j) == 0) {
                emptyCells.push_back({i, j});
            }
        }
    }

    if (emptyCells.empty()) return;

    // Chọn ngẫu nhiên 1 tọa độ trống
    int randomIndex = rand() % emptyCells.size();
    int row = emptyCells[randomIndex].first;
    int col = emptyCells[randomIndex].second;

    // Thuật toán tỷ lệ:
    // 15% ra chướng ngại vật (-1)
    // 75% ra số (2)
    // 10% ra số (4)
    int randomChance = rand() % 100;
    
    if (randomChance < 15) {
        // Tạo cục đá tại vị trí này (cần truy cập mảng board, 
        // lưu ý Thành viên 1 cần chuyển biến 'board' trong Grid.h từ private thành protected)
        board[row][col] = new ObstacleTile(row, col);
    } else {
        int newValue = (randomChance < 90) ? 2 : 4;
        board[row][col]->setValue(newValue);
    }
}
