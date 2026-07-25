#ifndef TILE_H
#define TILE_H

#include "IMoveable.h"

// Lớp Tile kế thừa từ Interface IMoveable (Tính Kế thừa)
class Tile : public IMoveable {
private:
    // Tính Đóng gói (Encapsulation): Ẩn giá trị bên trong
    int value;
    int x;
    int y;

public:
    // Constructor
    Tile(int startValue, int startX, int startY);

    // Cung cấp các phương thức truy cập an toàn (Getter/Setter)
    int getValue() const;
    void setValue(int newValue);
    
    int getX() const;
    int getY() const;

    // Ghi đè phương thức từ interface (Tính Đa hình có thể mở rộng từ đây)
    void move(int dx, int dy) override;
};

#endif // TILE_H
