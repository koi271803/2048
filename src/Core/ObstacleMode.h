#ifndef OBSTACLE_MODE_H
#define OBSTACLE_MODE_H

#include "Grid.h"

// 1. Tính Kế thừa: Lớp ObstacleTile kế thừa từ Tile cơ sở
class ObstacleTile : public Tile {
public:
    ObstacleTile(int startX, int startY) : Tile(-1, startX, startY) {}
    
    // Đảm bảo giá trị của cục đá luôn là -1, không thể bị thay đổi
    void setValue(int newValue) {
        // Không làm gì cả để chặn việc gộp số vào tảng đá
    }
};

// 2. Tính Kế thừa: Chế độ chơi mới kế thừa từ bàn cờ gốc
class ObstacleGameMode : public Grid {
public:
    // Khởi tạo mặc định kích thước 8x8 cho chế độ này
    ObstacleGameMode();

    // Tính Đa hình: Ghi đè hàm sinh số để thỉnh thoảng rớt ra tảng đá
    void spawnRandomTile() override;
};

#endif // OBSTACLE_MODE_H
