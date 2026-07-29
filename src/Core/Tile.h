#ifndef TILE_H
#define TILE_H

#include "IMoveable.h"

class Tile : public IMoveable {
protected:
    int value;
    int x; // Đại diện cho Row (Hàng)
    int y; // Đại diện cho Col (Cột)

public:
    Tile(int val, int startX, int startY);
    virtual ~Tile();

    virtual int getValue() const;
    virtual void setValue(int newValue);
    int getX() const;
    int getY() const;

    void move(int dx, int dy) override;
};

// THÊM MỚI: Lớp ObstacleTile kế thừa từ Tile
class ObstacleTile : public Tile {
private:
    int weight; // Trọng số từ 1 đến 5

public:
    // Gọi constructor lớp cha với value = -1 để đánh dấu đây là đá
    ObstacleTile(int startX, int startY, int initialWeight) : Tile(-1, startX, startY) {
        weight = initialWeight;
    }

    // Hàm nhận sát thương
    void takeDamage() {
        if (weight > 0) {
            weight--;
            if (weight == 0) {
                // Đá vỡ thì biến thành ô trống bình thường
                this->value = 0;
            }
        }
    }

    int getWeight() const { return weight; }
};

#endif // TILE_H
