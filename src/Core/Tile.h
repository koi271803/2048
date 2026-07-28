#ifndef TILE_H
#define TILE_H

#include "IMoveable.h"

// Tính Kế thừa: Tile kế thừa IMoveable
class Tile : public IMoveable {
protected: 
    int value;
    int x;
    int y;

public:
    Tile(int val, int startX, int startY);
    virtual ~Tile();

    // Tính Đóng gói: Getter và Setter
    virtual int getValue();
    virtual void setValue(int newValue);

    // THÊM MỚI: Khai báo 2 hàm getX và getY để khớp với file .cpp
    int getX() const;
    int getY() const;

    // Tính Đa hình: Ghi đè hàm move từ interface
    void move(int dx, int dy) override;
};

#endif // TILE_H
