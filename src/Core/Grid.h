#ifndef GRID_H
#define GRID_H

#include "Tile.h"

class Grid {
protected:
    int gridSize;
    int score;
    Tile*** board;

    // --- THÊM MỚI: Dữ liệu cho Power-ups ---
    int undoCount;
    int swapCount;
    int deleteCount;

    // Mảng và điểm lưu lại trạng thái trước đó (dành cho Undo)
    int** prevBoard;
    int prevScore;
    bool canUndo; // Cờ kiểm tra xem có trạng thái cũ để lùi về không

    int stonesBroken; // Đếm số đá đã phá trong màn
    void damageAdjacentObstacles(int row, int col); // Hàm kiểm tra 4 hướng để trừ máu đá

    bool pushLine(Tile** line);
    void saveState(); // Hàm nội bộ lưu trạng thái bàn cờ trước khi di chuyển

public:
    Grid(int size);
    virtual ~Grid();

    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();
    virtual void spawnRandomTile();
    bool checkGameOver();

    // --- THÊM MỚI: Các hàm sử dụng Power-ups ---
    bool useUndo();
    bool useSwap(int r1, int c1, int r2, int c2);
    bool useDelete(int r, int c);

    // --- THÊM MỚI: Các Getter để hiển thị lên màn hình ---
    int getUndoCount() const;
    int getSwapCount() const;
    int getDeleteCount() const;

    int getStonesBroken() const { return stonesBroken; } // Getter cho UI

    int getScore() const;
    int getTileValue(int row, int col) const;
    int getSize() const;
};

#endif // GRID_H
