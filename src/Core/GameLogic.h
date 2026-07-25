#ifndef GRID_H
#define GRID_H

#include "Tile.h"
#include <vector>

class Grid {
private:
    std::vector<std::vector<Tile*>> board;
    int gridSize;
    int score;
    bool isGameOver;

    // Các hàm nội bộ không cho bên ngoài gọi (Tính Đóng gói)
virtual void spawnRandomTile();
    void pushLine(std::vector<int>& line); // Dựa trên hàm push trong ảnh của bạn

public:
    Grid(int size);
    ~Grid();

    // Các thao tác điều khiển
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();

    // Các hàm để UI lấy dữ liệu hiển thị
    int getScore() const;
    bool checkGameOver();
    int getTileValue(int row, int col) const;
    int getSize() const;
};

#endif // GRID_H
