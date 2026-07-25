#include "Tile.h"

// Khởi tạo ô số
Tile::Tile(int startValue, int startX, int startY) {
    value = startValue;
    x = startX;
    y = startY;
}

// Tính Đóng gói: Lấy giá trị
int Tile::getValue() const {
    return value;
}

// Tính Đóng gói: Gán giá trị mới (khi gộp số)
void Tile::setValue(int newValue) {
    value = newValue;
}

int Tile::getX() const { return x; }
int Tile::getY() const { return y; }

// Tính Đa hình: Ghi đè hàm di chuyển
void Tile::move(int dx, int dy) {
    x += dx;
    y += dy;
}
