#ifndef GRID_H
#define GRID_H

#include "Tile.h"
#include <vector>

class Grid {
private:
    int size;                               // Kích thước của bảng
    std::vector<std::vector<Tile*>> board;          // Ma trận 2 chiều lưu trữ các ô tile hiện tại trên bàn cờ
    std::vector<std::vector<int>> previousBoard;    // Lưu trữ trạng thái bảng trước đó (phục vụ cho tính năng Undo)

    int score;                              // Tổng điểm số hiện tại của người chơi
    int previousScore;                      // Điểm số của trạng thái trước đó (phục vụ Undo điểm)
    int moveCount;                          // Tổng số lần di chuyển đã thực hiện

    int undoCount;                          // Số lần cho phép đi lại bước trước (Undo)
    int swapCount;                          // Số lần cho phép đổi vị trí 2 ô (Swap)
    int deleteCount;                        // Số lần cho phép xóa 1 ô tùy chọn (Delete)

    std::vector<int> lastMergedValues;      // Lưu giá trị của các ô vừa được gộp trong lượt gần nhất (dùng để hiệu ứng hoặc tính điểm)

    void saveState();                       // Hàm nội bộ: Lưu lại trạng thái bảng và điểm số hiện tại trước khi thực hiện nước đi mới
    void cleanupBoard();                    // Hàm nội bộ: Giải phóng bộ nhớ của các đối tượng Tile đang cấp phát động trên bảng

public:
    Grid(int boardSize = 4);

    ~Grid();

    void initBoard();                       // Khởi tạo lại bàn cờ từ đầu (xóa sạch hoặc cấp phát mới các ô trống)
    void spawnTile();                       // Sinh ngẫu nhiên một ô mới (giá trị 2 hoặc 4) vào ô trống trên bàn cờ

    // --- Các hàm xử lý hướng di chuyển của các ô (Logic chính của game) ---
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();

    // --- Các tính năng đặc biệt (Power-ups) ---
    bool useUndo();                         // Hoàn tác về trạng thái nước đi trước đó
    bool useSwap(int r1, int c1, int r2, int c2); // Đổi chỗ giá trị/ô giữa 2 vị trí (r1, c1) và (r2, c2)
    bool useDelete(int r, int c);           // Xóa giá trị của ô tại tọa độ hàng r, cột c

    // --- Các hàm Getter (Lấy thông tin trạng thái) ---
    int getScore() const;                   // Lấy điểm số hiện tại
    int getMoveCount() const;               // Lấy tổng số bước di chuyển
    int getTileValue(int r, int c) const;   // Lấy giá trị của ô tại tọa độ (r, c)
    int getMaxTile() const;                 // Lấy giá trị ô lớn nhất hiện có trên bàn cờ
    bool isGameOver() const;                // Kiểm tra xem game đã kết thúc chưa (hết nước đi và không còn ô trống)

    // Lấy số lượng quyền năng còn lại
    int getUndoCount() const;
    int getSwapCount() const;
    int getDeleteCount() const;

    // --- Các hàm Setter và Quản lý Power-ups ---
    void addUndo(int amount = 1) { undoCount += amount; }       // Cộng thêm lượt Undo
    void addSwap(int amount = 1) { swapCount += amount; }       // Cộng thêm lượt Swap
    void addDelete(int amount = 1) { deleteCount += amount; }   // Cộng thêm lượt Delete

    /**
     * @brief Thiết lập số lượng ban đầu cho các quyền năng (thường dùng cho chế độ Challenge).
     */
    void setPowerCounts(int undo, int swap, int del) {
        undoCount = undo;
        swapCount = swap;
        deleteCount = del;
    }

    const std::vector<int>& getLastMergedValues() const { return lastMergedValues; } // Lấy danh sách giá trị vừa gộp
    void clearLastMergedValues() { lastMergedValues.clear(); }                      // Xóa danh sách giá trị vừa gộp

    // --- Xử lý hiệu ứng đồ họa (Animations) ---
    void updateAnimations();                // Cập nhật trạng thái chuyển động/hiệu ứng của các ô theo thời gian thực
    float getTileScale(int r, int c) const; // Lấy tỉ lệ phóng to/thu nhỏ (scale) của ô tại (r, c) để vẽ hiệu ứng xuất hiện/gộp ô
};

#endif
