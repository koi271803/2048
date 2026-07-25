#ifndef GRID_H
#define GRID_H

#include "Tile.h"
#include <vector>

class Grid {
private:
    // Quản lý mảng 2 chiều 4x4 chứa các con trỏ Tile
    Tile* board[4][4];
    int score;
    bool isGameOver;

    // Các hàm tiện ích dùng nội bộ (Đóng gói)
    void spawnRandomTile();
    bool canMove();

public:
    Grid();
    ~Grid();

    // Các hàm nhận lệnh từ UI truyền vào
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();

    // Các hàm để UI lấy dữ liệu ra vẽ màn hình
    int getScore() const;
    bool checkGameOver() const;
    int getTileValue(int row, int col) const;
};

#endif // GRID_H
