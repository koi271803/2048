#include "Tile.h"

// Constructor mặc định (khởi tạo ô trống)
Tile::Tile() : value(0), x(0), y(0) {}

// Constructor khởi tạo ô có giá trị và vị trí cụ thể
Tile::Tile(int val, int startX, int startY) : value(val), x(startX), y(startY) {}

// Lấy giá trị của ô
int Tile::getValue() const {
    return value;
}

// Thay đổi giá trị của ô
void Tile::setValue(int val) {
    value = val;
}

// Lấy vị trí hàng
int Tile::getX() const {
    return x;
}

// Lấy vị trí cột
int Tile::getY() const {
    return y;
}

// Hàm thực thi việc di chuyển (Cập nhật vị trí của ô)
void Tile::move(int dx, int dy) {
    x += dx;
    y += dy;
}