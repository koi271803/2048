#include "Tile.h"

Tile::Tile(int val, int startX, int startY) {
    value = val;
    x = startX;
    y = startY;
}

Tile::~Tile() {}

int Tile::getValue() {
    return value;
}

// Tính Đóng gói: Gán giá trị mới (khi gộp số)
void Tile::setValue(int newValue) {
    value = newValue;
}

// Lỗi E0135 và E1670 sẽ biến mất vì đã được khai báo bên .h
int Tile::getX() const { return x; }
int Tile::getY() const { return y; }

// Tính Đa hình: Ghi đè hàm di chuyển
void Tile::move(int dx, int dy) {
    x += dx;
    y += dy;
}
