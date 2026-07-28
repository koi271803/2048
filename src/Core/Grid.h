#ifndef GRID_H
#define GRID_H

#include "Tile.h"

class Grid {
protected:
    int gridSize; // Sử dụng tên biến gridSize như trong code cũ của bạn
    int score;
    Tile*** board; // Mảng 2 chiều động chứa con trỏ Tile

    // Hàm nội bộ xử lý dồn số (Đóng gói)
    bool pushLine(Tile** line);

public:
    Grid(int size);
    virtual ~Grid();

    // Các thao tác di chuyển
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();

    // Tính Đa hình (sẵn sàng cho lớp ObstacleMode ghi đè)
    virtual void spawnRandomTile();

    bool checkGameOver();

    // Các hàm Getter (có từ khóa const giống y như trong ảnh của bạn)
    int getScore() const;
    int getTileValue(int row, int col) const;
    int getSize() const;
};

#endif // GRID_H
